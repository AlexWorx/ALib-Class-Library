//==================================================================================================
/// \file
/// This header-file is part of the module \alib_enumrecords of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace enumrecords {

#include "ALib.Lang.CIFunctions.H"
//##################################################################################################
// Parsing (Consume)
//##################################################################################################

//==================================================================================================
/// Consumes an element value of a C++ enumeration that is equipped with
/// \ref alib_enums_records "ALib Enum Records" of type \alib{enumrecords;ERSerializable} (or of a derived
/// type) from a given \alib{strings;TSubstring;Substring}.
///
/// In debug-builds, the method asserts that at least one record is defined for \p{TEnum}.
///
/// For more information, consult chapter
/// \ref alib_enums_records_details_serialization "4.3.1 Serialization/Deserialization" of the
/// Programmer's Manual of the module \alib_enumrecords_nl.
///
/// \note
///   This namespace function is applicable to \alib{enumops;BitwiseTraits;bitwise enumrecords} as well.
///   However, only one element name is parsed.
///   To parse multiple elements (ored to one resulting enum value), use sibling function
///   \alib{enumrecords;ParseBitwise}.
///
/// @tparam TEnum              The enumeration type equipped with <b>ALib Enum Recorss</b>
///                            of type \alib{enumrecords;ERSerializable}.
/// @tparam TChar              The character type of the \p{input} substring.
///                            Deduced by the compiler.
/// @tparam TSensitivity       The sensitivity of the comparison.
///                            Defaults to \b Case::Sensitive.
/// @tparam TTrimBeforeConsume Determines if the sub string should be (left-) trimmed before the
///                            consume operation. If so, in case of parsing failure, trimming is
///                            \b not restored.<br>
///                            Defaults to \b Whitespaces::Trim.
/// @param[in,out] input       The substring to parse. Passed as reference. On success, the
///                            substring will be shortened by the characters consumed. On failure
///                            only trimmed characters are consumed.
/// @param[out]    result      The result enum element given as reference.
/// @return \c true if an enum element was successfuly recognized, \c false otherwise.
///
/// ### Module Dependencies ###
///   This method is only available if the module \alib_strings is included in the \alibbuild.
//==================================================================================================
template<typename          TEnum,
         typename          TChar,
         lang::Case        TSensitivity        = lang::Case::Ignore,
         lang::Whitespaces TTrimBeforeConsume  = lang::Whitespaces::Trim >
requires alib::enumrecords::IsSerializable<TEnum>
bool Parse( strings::TSubstring<TChar>& input,  TEnum&  result ) {
    ALIB_ASSERT_ERROR( EnumRecords<TEnum>().begin() != EnumRecords<TEnum>().end(), "ENUMS",
                          "No Enum Records for type <{}> found.", &typeid(TEnum) )
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

//==================================================================================================
/// Repeatedly invokes sibling function \alib{enumrecords;Parse} until \p{delim} is not found.
/// The enum element values are or'ed in \p{result}.
///
/// In debug-builds, the method asserts that at least one record is defined for \p{TEnum}.
///
/// \note
///   This method is applicable only to \alib{enumops;BitwiseTraits;bitwise enumrecords} that likewise
///   are equipped with \ref alib_enums_records "ALib Enum Records" of (derived) type
///   \alib{enumrecords;ERSerializable}.
///
///
/// @tparam TEnum              The enumeration type equipped with <b>ALib Enum Records</b>
///                            of (derived) type \alib{enumrecords;ERSerializable} and furthermore with a
///                            specialization of \alib{enumops;BitwiseTraits}.
/// @tparam TChar              The character type of the \p{input} substring.
///                            Deduced by the compiler.
/// @tparam TSensitivity       The sensitivity of the comparison.
///                            Defaults to \b Case::Ignore.
/// @tparam TTrimBeforeConsume Determines if the substring should be (left-) trimmed before and
///                            after each consume operation. If so, in case of parsing failure,
///                            trimming is \b not restored.<br>
///                            Defaults to \b Whitespaces::Trim.
/// @tparam delimiter          The delimiter character of the enum elements.<br>
///                            Defaults to <c>','</c>.
/// @tparam keepLastDelim      If \c true , the delimiter will be kept in this substring, if
///                            after the delimiter no further enum element was found.
///                            If \c false, the delimiter will be kept.<br>
///                            Defaults to \c true.
/// @param[in,out] input       The substring to parse. Passed as reference. On success, the
///                            substring will be shortened by the characters consumed. On failure
///                            only trimmed characters are consumed.
/// @param[out] result         The result enum element given as reference.
/// @return \c true if an enum element was successfuly recognized, \c false otherwise.
///
/// ### Module Dependencies ###
///   This method is only available if the module \alib_strings is included in the \alibbuild.
//==================================================================================================
template<typename          TEnum,
         typename          TChar,
         lang::Case        TSensitivity       = lang::Case::Ignore,
         lang::Whitespaces TTrimBeforeConsume = lang::Whitespaces::Trim,
         TChar             delimiter          = ',',
         bool              keepLastDelim      = true                  >
requires (     alib::enumrecords::IsSerializable<TEnum>
            && alib::enumops::IsBitwise     <TEnum> )
bool ParseBitwise( strings::TSubstring<TChar>& input, TEnum&  result ) {
    bool mResult= false;
    result= TEnum(0);
    strings::TSubstring<TChar> restoreBeforeDelim;
    if constexpr ( keepLastDelim )
        restoreBeforeDelim= input;
    for(;;) {
        if constexpr ( TTrimBeforeConsume == lang::Whitespaces::Trim )
            input.TrimStart();
        TEnum actEnum;
        if ( !Parse<TEnum, TChar, TSensitivity, TTrimBeforeConsume>( input, actEnum ) ) {
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

}   }

//==================================================================================================
/// Convenience method that first uses \alib{enumrecords;Parse} to try and read an element of a C++
/// enum. If this is not successful, an enum of type \alib{lang;Bool} is tried to be read.
/// If this is successful, depending on the value read, the \p{TEnum} values given
/// as parameters \p{falseValue} and \p{trueValue} are assigned.
/// Otherwise, \c false is returned.
///
/// In debug-builds, the method asserts that at least one record is defined for \p{TEnum}.
///
/// \see
///   For more information, consult chapter \ref alib_enums_records_details_serialization of the
///   Programmer's Manual of the module \alib_resources.
///
/// @tparam TEnum              The enumeration type equipped with <b>ALib Enum Records</b>
///                            of (derived) type \alib{enumrecords;ERSerializable}.
/// @tparam TChar              The character type of the \p{input} substring.
///                            Deduced by the compiler.
/// @tparam TSensitivity       The sensitivity of the comparison.
///                            Defaults to \b Case::Ignore.
/// @tparam TTrimBeforeConsume Determines if the substring should be (left-) trimmed before and
///                            after each consume operation. If so, in case of parsing failure,
///                            trimming is \b not restored.<br>
///                            Defaults to \b Whitespaces::Trim.
/// @param  trueValue          The \p{TEnum} value to use in case of \c Bool::True was read.
/// @param  falseValue         The \p{TEnum} value to use in case of \c Bool::False was read.
/// @param[in,out] input       The substring to parse. Passed as reference. On success, the
///                            substring will be shortened by the characters consumed. On failure
///                            only trimmed characters are consumed.
/// @param[out] result         The result enum element given as reference.
/// @return \c true if an element of \p{TEnum} or \alib{lang;Bool} could be read,
///         \c false otherwise.
//==================================================================================================
template<typename          TEnum,
         typename          TChar,
         lang::Case        TSensitivity       = lang::Case::Ignore,
         lang::Whitespaces TTrimBeforeConsume = lang::Whitespaces::Trim >
requires alib::enumrecords::IsSerializable<TEnum>
bool ParseEnumOrTypeBool( strings::TSubstring<TChar>&   input,
                          TEnum&                        result,
                          TEnum                         falseValue,
                          TEnum                         trueValue         ) {
    // first try to read a TEnum
    if( Parse<TEnum, TChar, TSensitivity, TTrimBeforeConsume>( input, result ) )
        return true;

    // if failed, read boolean
    lang::Bool boolEnum;
    if( enumrecords::Parse<lang::Bool, TChar, TSensitivity, lang::Whitespaces::Keep>( input, boolEnum ) ) {
        result= (boolEnum == lang::Bool::True) ? trueValue : falseValue;
        return true;
    }

    // failed
    return false;
}

}} // namespace [alib::enumrecords]



//##################################################################################################
// Writing (AppendableTraits<Enum>)
//##################################################################################################

namespace alib {  namespace strings {

// Faking specializations of AppendableTraits for doxygen into namespace alib::strings::APPENDABLES
#if DOXYGEN
    namespace APPENDABLES {
#endif

#if !ALIB_RESOURCES || DOXYGEN

/// Templated specialization of functor \alib{strings;AppendableTraits} makes elements
/// of C++ enumeration type \p{TEnum} appendable to \alib{strings;TAString;AString} instances if:
/// - Type \p{TEnum} is equipped with \ref alib_enums_records "ALib Enum Records" of type
///   \alib{enumrecords;ERSerializable} (or a derived type ), and if:
/// - \b No specialization of the type trait \alib{enumops;BitwiseTraits} exists for \p{TEnum}.
///   (For those, a different implementation of this functor is given.)
///
/// \note
///    The conditions are evaluated by the compiler using keyword \c requires and concepts
///    \alib{enumrecords;IsSerializable} and \alib{enumops;IsBitwise} to generically specialize
///    the type trait \alib{strings;AppendableTraits} of the module \alib_strings.
///
/// Member \b operator() writes the name of the given enumeration value to \p{target}.
/// As with all specializations of functor \b %AppendableTraits, the use of it is done implicitly
/// with various interface method of class \alib{strings;TAString;AString}.
///
/// If furthermore, the type trait \alib{resources;ResourcedTraits} is specialized for
/// type \p{TEnum}, and an externalized resource string exists - according to the specification
/// described with methods \alib{resources;ResourcedType::TypeNamePrefix} and
/// \alib{resources;ResourcedType::TypeNamePostfix} - then these resourced strings are written
/// prior and after the enumeration element's name.
///
/// \par Availability
///  For technical reasons, this functor is available with the inclusion of the header
///  \implude{Resources}, instead of the header \implude{EnumRecords}.<br>
///  Only in the case that the module \alib_resources is \b not included in the \alibbuild, then a
///  reduced version which does not display a  resourced enumeration type name, is defined with
///  the module \alib_enumrecords_nl.
///
/// \see
///   - Sibling specialization
///     \alib{strings::APPENDABLES;AppendableTraits<TBitwiseEnum,TChar,TAllocator>}
///   - \ref alib_enums_records "ALib Enum Records".
///   - Type trait \alib{resources;ResourcedTraits}.
///   - Corresponding convenience type  \alib{resources;ResourcedType}.
///   - Some sample code is given with documentation \ref alib_enums_records "ALib Enum Records".
///
/// @tparam TEnum      The enumeration type of the element that is to be appended to an \b %AString.
/// @tparam TChar      The character type of the target \b %AString.
/// @tparam TAllocator The allocator type of the target \b %AString, as prototyped with
///                    \alib{lang;Allocator}.
template<typename TEnum, typename TChar, typename TAllocator>
requires (  enumrecords::IsSerializable<TEnum> && !enumops::IsBitwise<TEnum> )
struct  AppendableTraits<TEnum, TChar, TAllocator>
{
    //==============================================================================================
    /// Writes the name of the given enumeration \p{element} it to \p{target}.
    ///
    /// If available for \p{TEnum}, the resourced name
    /// \alib{resources;ResourcedType::TypeNamePrefix;prefix} and
    /// \alib{resources;ResourcedType::TypeNamePostfix;postfix} are written before and after
    ///  the element's name.
    ///
    /// If no record exists for \p{element}, its underlying integral value is written.
    /// In debug-builds, the method asserts that at least one record is defined for \p{TEnum}.
    ///
    ///
    /// @param target    The \b AString that \p{element} is to be appended to.
    /// @param element   The enumeration element to append to \p{target}.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target, TEnum element )
    {
        ALIB_ASSERT_ERROR( EnumRecords<TEnum>().begin() != EnumRecords<TEnum>().end(), "ENUMS",
                             "No Enum Records for type <{}> found.", &typeid(TEnum) )

        auto* record= enumrecords::TryRecord( element );
        if( record != nullptr )
            target << record->EnumElementName;
        else
            target << UnderlyingIntegral( element );
    }
};

/// This specialization of type-traits-functor \alib{strings;AppendableTraits} makes elements of
/// C++ enumeration type \p{TBitwiseEnum} appendable to \alib{strings;TAString;AString} instances,
/// if
/// - Type \p{TBitwiseEnum} is equipped with \ref alib_enums_records "ALib Enum Records" of type
///   \alib{enumrecords;ERSerializable} (or a derived type derived from \b ERSerializable), and
/// - \b A specialization of type trait \alib{enumops;BitwiseTraits} exists for \p{TBitwiseEnum}.
///   (For non-bitwise types, a different implementation of this functor is given.)
///
/// Member \b operator() writes all value names corresponding to the bits set in \p{src},
/// separated by delimiter character <c>','</c>.
/// \note
///    For technical reasons, the delimiter is not adjustable. In cases a different delimiter
///    is to be used, it needs to be replaced in the string after the value is written.
///
/// If the underlying integral value of a given enum element may be become \c 0, a corresponding
/// enum record associated to such non-bit value will be used if existent.
///
/// Furthermore, with bitwise type enumrecords, the defined enum records may contain entries that
/// represent combinations of more than one integral bit. Such combination entries are supported
/// but have to be registered with class \alib{enumrecords;EnumRecords} \b before the
/// standard single bit entries!
/// If combined bit values are matched, the corresponding element names that represent the
/// single bit values will not be written.
///
/// As a sample, let's consider a window manager software which has a scoped enum type representing
/// window states. Because a window might have more than one state, macro
/// \ref ALIB_ENUMS_MAKE_BITWISE is used to specialize the type trait \alib{enumops;BitwiseTraits}.
/// Next, macro \ref ALIB_ENUMS_ASSIGN_RECORD used to associate the type with enum record type
/// \alib{enumrecords;ERSerializable}:
///
///  \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_BITWISE_DECLARATION
///
/// A window that is both, vertically and horizontally maximized, is considered to be "just"
/// \e maximized. Therefore, during bootstrap of the software, enum records that fetch that the
/// combination of states are defined \b before the single-state records:
///
///  \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_BITWISE_DEFINITION
///
/// That is all that is needed! With this setup, the following code:
///  \snippet "DOX_ENUMS.cpp"     DOX_ENUMS_BITWISE_SAMPLE
///
/// produces this output:
///  \snippet "DOX_ENUMS_BITWISE_OUTPUT.txt"     OUTPUT
///
/// If furthermore, the type trait \alib{resources;ResourcedTraits} is specialized for
/// type \p{TBitwiseEnum} and an externalized resouce string exists - according to the
/// specification described with the methods \alib{resources;ResourcedType::TypeNamePrefix}
/// and /// \alib{resources;ResourcedType::TypeNamePostfix} - then these resourced strings
/// are written prior and after the enumeration element name(s).
///
/// \par Availability
///  For technical reasons, this functor is available with the inclusion of the header
///  \implude{Resources}, instead of the header \implude{EnumRecords}.<br>
///  Only in the case that the module \alib_resources is \b not included in the \alibbuild, then a
///  reduced version which does not display a  resourced enumeration type name, is defined with
///  the module \alib_enumrecords_nl.
///
/// \see
///   - Sibling specialization
///     \alib{strings::APPENDABLES;AppendableTraits<TEnum,TChar,TAllocator>}
///   - The type trait \alib{resources;ResourcedTraits}.
///   - Corresponding convenience type  \alib{resources;ResourcedType}.
///   - Some sample code is given with documentation \ref alib_enums_records "ALib Enum Records".
///
/// # Reference Documentation #
/// @tparam TBitwiseEnum  The bitwise defined enumeration type.
/// @tparam TChar         The character type of the target \b %AString.
/// @tparam TAllocator    The allocator type of the target \b %AString, as prototyped with
///                       \alib{lang;Allocator}.
template<typename TBitwiseEnum, typename TChar, typename TAllocator>
requires (     alib::enumrecords::IsSerializable<TBitwiseEnum>
            && alib::enumops::IsBitwise     <TBitwiseEnum>  )
struct  AppendableTraits<TBitwiseEnum, TChar,TAllocator> {
    //==============================================================================================
    /// Writes a comma-separated list of element names of the bitwise defined enumeration \p{TEnum}
    /// to \p{target}.
    ///
    /// In debug-builds, the method asserts that at least one record is defined for \p{TEnum}.
    /// It is furthermore asserted that all bits contained in \p{elements} have been "covered"
    /// by corresponding names.
    ///
    /// The enum records defined may aggregate several bits. Aggregations have to be defined
    /// before records that represent the corresponding single bits (or another subset of those).
    ///
    /// \see
    ///   This struct's documentation for more information and a sample.
    ///
    /// @param target    The \b AString that \p{elements} is to be appended to.
    /// @param elements  The enumeration element to append to \p{target}.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target, TBitwiseEnum elements )
    {
        ALIB_ASSERT_ERROR( EnumRecords<TBitwiseEnum>().begin() != EnumRecords<TBitwiseEnum>().end(),
                    "ENUMS",  "No Enum Records for type <{}> found.", &typeid(TBitwiseEnum) )

        // check what has been covered and omit double entries
        TBitwiseEnum covered= TBitwiseEnum(0);

        // loop over entry 2 to end, check bit
        integer len= target.Length();

        for( auto recordIt=   EnumRecords<TBitwiseEnum>().begin() ;
                  recordIt != EnumRecords<TBitwiseEnum>().end()   ; ++recordIt  )
        {
            // no bits are set and this entry does not contain bits, then stop here
            if( recordIt.Integral() == 0 )
            {
                if( elements == TBitwiseEnum(0) )
                {
                    target << recordIt->EnumElementName;
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

        ALIB_ASSERT_ERROR( covered == elements, "ENUMS",
           "Not all bits have been covered while writing bitset '{}' of enumeration type <{}>."
                        " Remaining bits are '{}'.", NString128(NBin( elements )),
                             &typeid(TBitwiseEnum),  NString128(NBin( covered & elements ))  )
    }
};

#endif // !ALIB_RESOURCES || DOXYGEN

#if DOXYGEN
} // namespace alib::strings[::appendables]
#endif
#include "ALib.Lang.CIMethods.H"

}} // namespace [alib::strings]

//==================================================================================================
/// \file
/// This header-file is part of the module \alib_enumrecords of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================

ALIB_EXPORT namespace alib::enumrecords::bootstrap {

#include "ALib.Lang.CIFunctions.H"

/// Helper-struct used with bulk-initialization function
///\doxlinkproblem{namespacealib_1_1enums_1_1bootstrap.html;ae0d44dc0bbb6a855bb44eb603b2b79c7;Bootstrap(std::initializer_list)}.
/// @tparam TEnum   The enum type to define records for.
template<typename TEnum>
    requires enumrecords::HasRecords<TEnum>
struct Initializer {
    /// The enum's associated record type.
    using TRecord = typename RecordsTraits<TEnum>::Type;

    /// The enumeration element.
    TEnum element;

    /// The static data record.
    TRecord record;

    /// Constructor taking variadic template arguments to construct the record.
    ///
    /// @tparam TArgs  Types of the variadic arguments \p{args}.
    /// @param  elem   The enum element.
    /// @param  args   Variadic arguments forwarded to constructor of field #record.
    template<typename... TArgs>
    Initializer(TEnum elem, TArgs &&... args) noexcept
    : element(elem)
    , record(std::forward<TArgs>(args)...)                                                        {}
};

/// Defines a record for a single element of \p{TEnum}.
///
/// \note
///   This method is rather provided for completeness, than for actual use, because it is
///   preferred to bootstrap enum records as "bulk" data.
///   Furthermore, it is preferred to use overloaded versions that accept static string data
///   used to parse the data from. This is more efficient in respect to the footprint of
///   an application, and - if strings are \ref alib_mod_resources "resourced" -
///   far more flexible.
///
/// \see Chapter \ref alib_enums_records_firststep_init for a sample of how this method
///      can be invoked.
///
/// @tparam TArgs   Types of the variadic arguments \p{args}.
/// @tparam TEnum   The enum type to define records for.
/// @param  element The enum element.
/// @param  args    Variadic arguments forwarded to constructor of the custom record to
///                 create and store.
template<typename TEnum, typename... TArgs>
requires alib::enumrecords::HasRecords<TEnum>
void Bootstrap(TEnum element, TArgs &&... args)                                           noexcept {
    auto & records = detail::EnumRecordHook<TEnum>::GetSingleton();
    auto **lastP   = records.getPointerToLast();
    #if ALIB_MONOMEM
    *lastP = monomem::GLOBAL_ALLOCATOR()
    #else
        *lastP= HeapAllocator()()
    #endif
           .New<typename detail::EnumRecordHook<
                TEnum>::Node>(element, std::forward<TArgs>(args)...);

    detail::setEnumRecord(typeid(TEnum), integer(element), &(*lastP)->record);
    (*lastP)->next = nullptr;
}


/// Associates elements of \p{TEnum} with records, as specified by the given list of
/// \p{definitions}.
///
/// The use of struct \alib{enumrecords;bootstrap::Initializer} allows placing
/// the enumeration element together with the construction parameters of the custom record
/// type into one comma-separated argument list, without the need to place extra curly braces
/// around the arguments of the record.
/// (Such would have been necessary if, for example, <c>std::pair</c> had been used).
///
/// \note
///   It is preferred to use overloaded versions that parse definitions from
///   static string data. This is more efficient in respect to the footprint of
///   an application, and - if strings are \ref alib_mod_resources "resourced" -
///   far more flexible.
///
/// \see Chapter \ref alib_enums_records_firststep_init for a sample of how this method
///      can be invoked.
///
/// @param definitions List of static enum records to store.
template<typename TEnum>
requires alib::enumrecords::HasRecords<TEnum>
void Bootstrap(std::initializer_list<Initializer<TEnum> > definitions) {
    auto * table   = definitions.begin();
    auto & records = detail::EnumRecordHook<TEnum>::GetSingleton();
    auto **lastP   = records.getPointerToLast();
    for (size_t i = 0; i != definitions.size(); ++i) {
        #if ALIB_MONOMEM
        *lastP = monomem::GLOBAL_ALLOCATOR()
        #else
            *lastP= HeapAllocator()()
        #endif
               .New<typename detail::EnumRecordHook<
                    TEnum>::Node>(table[i].element, table[i].record);

        detail::setEnumRecord(typeid(TEnum), integer(table[i].element), &(*lastP)->record);
        lastP = &(*lastP)->next;
    }

    (*lastP) = nullptr;
}

/// Reads a list of enum data records from given string \p{input}.
///
/// The contents (buffer) of the given substring have to be of a static nature (by contract).
/// This means that parsing will not create copies of portions of the string but still
/// use them later.
/// Consequently, the given string's buffer has to survive the life-cycle of an application.
///
/// This is due to the static nature of \ref alib_enums_records "ALib Enum Records" and their
/// creation during bootstrap, either from C++ string literals or
/// \ref alib_mod_resources "ALib Externalized Resources", which comply to the same contract.
///
/// \par Availability
///   This method is available only if \alib_strings is included in the \alibbuild.
///
/// \see Chapter \ref alib_enums_records_resourced_parsing for a sample of how this method
///      can be invoked.
///
/// @param input      The string used for parsing the enum records to store.
/// @param innerDelim The delimiter used for separating the fields of a record.
///                   Defaults to <c>','</c>.
/// @param outerDelim The character delimiting enum records.
///                   Defaults to <c>','</c>.
template<typename TEnum>
requires alib::enumrecords::HasRecords<TEnum>
void Bootstrap(const String &input,
               character     innerDelim=',',
               character     outerDelim=',')    {
    EnumRecordParser::Initialize(input, innerDelim, outerDelim, NULL_NSTRING, NULL_NSTRING);

    auto & records = detail::EnumRecordHook<TEnum>::GetSingleton();
    auto **lastP   = records.getPointerToLast();

    for (;;) {
        #if ALIB_MONOMEM
        auto *element = (*lastP = monomem::GLOBAL_ALLOCATOR()
        #else
            auto* element=  (*lastP= HeapAllocator()()
        #endif
                        .New<typename detail::EnumRecordHook<TEnum>::Node>());
        EnumRecordParser::Get(element->integral);
        element->record.Parse();

        detail::setEnumRecord(typeid(TEnum), integer(element->integral), &element->record);

        // next?
        lastP = &element->next;
        if (EnumRecordParser::Input.IsEmpty())
            break;
        EnumRecordParser::OuterDelim();
    }
    EnumRecordParser::assertEndOfInput();
    (*lastP) = nullptr;
}

#include "ALib.Lang.CIMethods.H"
} // namespace [alib::enumrecords::bootstrap]

//##################################################################################################
// enumrecords::detail::bootstrap()/shutdown()
//##################################################################################################
ALIB_EXPORT namespace alib::enumrecords::detail {

//==================================================================================================
/// Frees resources and shuts down module \alib_enumrecords.
/// Multiple invocations of this method are forbidden.
/// The \ref alib_mod_bs "standard bootstrap" code of \alib, hence the (overloaded)
/// functions \ref alib::Shutdown will call this function.
//==================================================================================================
void shutdown();
} // namespace [alib::enumrecords::detail]

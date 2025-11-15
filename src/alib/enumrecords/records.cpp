//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/enumrecords/enumrecords.prepro.hpp"
#if !ALIB_MONOMEM || !ALIB_CONTAINERS
#    include <unordered_map>
#endif
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Lang;
#   if ALIB_MONOMEM && ALIB_CONTAINERS
      import ALib.Monomem;
      import ALib.Containers.HashTable;
#   endif
#else
#   include "ALib.EnumRecords.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Lang.H"
#   if ALIB_MONOMEM && ALIB_CONTAINERS
#     include "ALib.Monomem.H"
#     include "ALib.Containers.HashTable.H"
#   endif
#endif
//========================================== Implementation ========================================
namespace alib::enumrecords::detail {

namespace {
#if ALIB_MONOMEM && ALIB_CONTAINERS
    #if DOXYGEN
    /// Global directory to find enum records.
    HashMap           < MonoAllocator,
                        EnumRecordKey, const void*,
                        EnumRecordKey::Hash,
                        EnumRecordKey::EqualTo       >  ENUM_RECORD_MAP;
    #else
    HashMap           < MonoAllocator,
                        EnumRecordKey, const void*,
                        EnumRecordKey::Hash,
                        EnumRecordKey::EqualTo       >  ENUM_RECORD_MAP( monomem::GLOBAL_ALLOCATOR,
                                                                       3.0, 6.0  );
    #endif //if DOXYGEN
#else
    std::unordered_map< EnumRecordKey, const void*,
                        EnumRecordKey::Hash,
                        EnumRecordKey::EqualTo       >  ENUM_RECORD_MAP;
#endif
}

void  setEnumRecord( const std::type_info& rtti, integer elementValue, const void* record ) {
    #if ALIB_MONOMEM && ALIB_CONTAINERS
        ENUM_RECORD_MAP.EmplaceIfNotExistent( EnumRecordKey(rtti, elementValue ), record );
    #else
        ENUM_RECORD_MAP.try_emplace( EnumRecordKey(rtti, elementValue ), record );
    #endif
}

const void* getEnumRecord( const std::type_info& rtti, integer elementValue ) {

#if ALIB_MONOMEM && ALIB_CONTAINERS
    auto it= ENUM_RECORD_MAP.Find( EnumRecordKey( rtti, elementValue ) );
#else
    auto it= ENUM_RECORD_MAP.find( EnumRecordKey( rtti, elementValue ) );
#endif
    if ( it != ENUM_RECORD_MAP.end() )
        return it->second;

    return nullptr;
}

#if ALIB_MONOMEM && ALIB_CONTAINERS
HashMap           < MonoAllocator,
                    EnumRecordKey, const void*,
                    EnumRecordKey::Hash,
                    EnumRecordKey::EqualTo     >& getInternalRecordMap() { return ENUM_RECORD_MAP; }
#else
std::unordered_map< EnumRecordKey, const void*,
                    EnumRecordKey::Hash,
                    EnumRecordKey::EqualTo     >& getInternalRecordMap() { return ENUM_RECORD_MAP; }
#endif


} // namespace [alib::enumrecords::detail]


//##################################################################################################
// Implementation of parsing methods of built-in record types.
//##################################################################################################
namespace alib::enumrecords {
void ERSerializable::Parse() {
    bootstrap::EnumRecordParser::Get( EnumElementName     );
    bootstrap::EnumRecordParser::Get( MinimumRecognitionLength , true );
}}



//##################################################################################################
// EnumRecordPrototype (Doxygen only)
//##################################################################################################

#if DOXYGEN
namespace alib::enumrecords {
//==================================================================================================
/// This struct is \b not part of the library but only provided with the documentation of this
/// \alibmod_nl.
///
/// The struct prototypes what module \alib_enumrecords_nl \b expects from custom types that are
/// associated to enumerations as the type of  \ref alib_enums_records "ALib Enum Records".
///
/// Usually, enum records are rather simple structs with fields of scalar or
/// \https{POD types,en.cppreference.com/w/cpp/types/is_pod} like, \c int, \c double or
/// \alib{strings;TString;String} and this way remain POD types themselves.
///
/// When parsed or otherwise initialized, String members do not need to copy data to an
/// own buffer, because the input string for parsing
/// an instance, as well as the parameters of alternative constructors, are deemed to be static data.
///
/// The life-cycle of instances of enumeration record is from bootstrapping an application
/// to its termination. After creation, the data cannot be modified.
//==================================================================================================
struct EnumRecordPrototype
{
    /// Default constructor leaving the record undefined. For efficiency, this usually does
    /// not initialize fields, as those will be overwritten by a subsequent invocation to #Parse.
    ///
    /// This constructor is only needed when method #Parse is given. Note that it is advised to
    /// provide the parsing option and this way also this constructor.
    EnumRecordPrototype()                                                       noexcept  = default;

    /// Constructor accepting all necessary arguments to completely define the record.
    /// This constructor is needed only in the case that records should be defined in a statical
    /// way which is an alternative to implementing the definition by parsing an (externalized)
    /// initialization strings. Static definition can be performed with method
    /// \alib{enumrecords;EnumRecords;Bootstrap(std::initializer_list<Initializer> definitions)}
    /// but is not recommended, and the definition from parsable stings is preferred.
    ///
    /// Note that the parameter's passed when this constructor is invoked, have to be of
    /// "static nature". For example, the buffers and contents of passed string values are
    /// deemed to survive the life-cycle of an application. Usually, C++ string literals are passed.
    ///
    /// @param myArg1   Assigned to the first custom field.
    /// @param myArg2   Assigned to the second custom field.
    /// @param ...      Further parameters, assigned to further fields.
    EnumRecordPrototype(  const MyType1& myArg1, MyType2 myArg2, ... )                     noexcept;

    /// Implementation has to parse the fields of this record from static interface struct
    /// \alib{enumrecords::bootstrap;EnumRecordParser}.
    ///
    /// For - usually simple - enum records, the process of parsing is limited to reading
    /// values separated by delimiters. Convenient methods to do so are given by static type
    /// \alib{enumrecords::bootstrap;EnumRecordParser}. More complex parsing logic may be implemented by
    /// using the "parser" substring found with \alib{enumrecords::bootstrap;EnumRecordParser::Input} and further
    /// of its entities.<br>
    /// Please refer to the documentation of \alib{enumrecords::bootstrap;EnumRecordParser} for all details.
    /// A source code sample is given in chapter \ref alib_enums_records_resourced_parsing of the
    /// Programmer's Manual of the module \alib_enumrecords_nl.
    ///
    /// The contents (buffer) of the string parsed is by contract of static nature. This means
    /// that no copies of portions need to be allocated when used as a field value of string-type.
    /// This is in alignment with the static nature of \ref alib_enums_records "ALib Enum Records"
    /// and their creation during bootstrap, either from C++ string literals or
    /// \ref alib_mod_resources "ALib Externalized Resources", which comply to the same contract.
    ///
    /// By the same token, in case of an error, an implementation should raise an exception in
    /// debug-compilations, as parsing is deemed to succeed on static data, even if externalized.
    ALIB_DLL
    void Parse()                                                                           noexcept;
}; // EnumRecordPrototype

} // namespace [alib::enumrecords]
#endif // DOXYGEN

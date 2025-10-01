//==================================================================================================
/// \file
/// This header-file is part of the module \alib_enumrecords of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace enumrecords {

// #################################################################################################
// RecordsTraits
// #################################################################################################

//==================================================================================================
/// This type trait is used to associate an <b><em>"ALib Enum Record"</em></b> type to a
/// (scoped or non-scoped) enumeration type.
///
/// In the non-specialized version type definition #Type (the only entity of this struct)
/// evaluates to <c>void</c>. To assign a record, specify a data record type, similar to what
/// is prototyped with \alib{enumrecords;EnumRecordPrototype}
///
/// \see
///   Please consult chapter \ref alib_enums_records "4. Enum Records" of the Programmer's Manual
///   of \alib_enumrecords_nl for detailed documentation and sample code on this struct and
//    underlying concept.
///
/// \see
///   Macro \ref ALIB_ENUMS_ASSIGN_RECORD offers a well readable alternative to specialize this
///   struct for an enum type.
///
/// @tparam TEnum      The enumeration type this struct applies to.
//==================================================================================================
template<typename TEnum>
requires std::is_enum<TEnum>::value
struct RecordsTraits
{
    /// The data type associated with elements of \p{TEnum}.
    using  Type=   void;
};


ALIB_WARNINGS_IGNORE_DOCS

/// Concept that is satisfied if the type trait \alib{enumrecords;RecordsTraits}
/// is specialized for type \p{TEnum} which specifies type alias \b Type.
/// @tparam TEnum The type to test.
template<typename TEnum>
concept HasRecords = !std::same_as<void, typename RecordsTraits<TEnum>::Type>;

ALIB_WARNINGS_RESTORE


// #################################################################################################
// detail {}
// #################################################################################################
/// Details of namespace #alib::enumrecords.
namespace detail {

// #################################################################################################
// Detail functions
// #################################################################################################
//==================================================================================================
/// Stores the \p{record} for enum \p{element} of the given enum type \p{rtti}.
/// If a value already exists, it is ignored. This allows having multiple records, while only
/// the first is found.
/// @param rtti     The enumeration type.
/// @param integral The integral value of the enum element.
/// @param record   The record to associate.
//==================================================================================================
ALIB_DLL
void        setEnumRecord( const std::type_info& rtti, integer integral, const void* record );

//==================================================================================================
/// Retrieves the enum record stored for the enum element with integral value \p{integral} of
/// enum type \p{rtti}.
/// @param rtti     The enumeration type.
/// @param integral The integral value of the enum element.
/// @return A pointer to the record, \c nullptr if not found.
//==================================================================================================
ALIB_DLL
const void* getEnumRecord( const std::type_info& rtti, integer integral );


//==================================================================================================
/// This is the internal singleton that provides a link to the first
/// \ref alib_enums_records "ALib Enum Record".
/// The class inherit's \alib{singletons;Singleton} and has a private default constructor, what
/// makes this type a \ref alib_singleton_strict "strict singleton".
///
/// Because enum record types are ensured to be trivially destructible types, no destructor is given.
///
/// @tparam TEnum  The enumeration type associated with enum records.
//==================================================================================================
template<typename TEnum>
struct EnumRecordHook :  Singleton<EnumRecordHook<TEnum>>
{
    #if !DOXYGEN
        friend alib::Singleton<EnumRecordHook>;
    #endif

    /// The enum's underlying integer type.
    using  TIntegral=  typename std::underlying_type<TEnum>::type;

    /// The enum's associated record type.
    using  TRecord  =  typename        RecordsTraits<TEnum>::Type;


    /// A node of the forward list that contains the custom record data
    struct Node
    {
        /// The enum element's underlying integral value.
        TIntegral   integral;

        /// The data record.
        TRecord     record;

        /// Pointer to the next node.
        Node*       next;

        /// Constructs this instance taking variadic template arguments to construct the contained
        /// enum record of custom type.
        ///
        /// @tparam TArgs   Types of the variadic arguments \p{args}.
        /// @param  element The enum element
        /// @param  args    Variadic arguments forwarded to constructor of field #record.
        template <typename... TArgs>
        Node( TEnum element, TArgs&&... args)                                   noexcept
        : integral( TIntegral(element) )
        , record  ( std::forward<TArgs>(args)... )
        {}

        /// Default constructor.
        Node()                                                                  noexcept   =default;

    };

    /// The hook to the first record defined.
    Node*    first;

    /// Helper methods that returns the address of field
    /// \alib{enumrecords::detail;EnumRecordHook::Node::next} the last element contained in the
    /// list. If no elements have been initialized, yet, the address of field #first is
    /// returned.
    /// @return  A pointer to the pointer of the last element or this hook.
    Node** getPointerToLast()
    {
        auto** last= &first;
        while( (*last) != nullptr )
            last= &(*last)->next;
        return last;
    }

    private:
        /// Private default constructor, what makes this type a
        /// \ref alib_singleton_strict "strict singleton".
        ///
        ///  \note As enum record types are trivially destructible types, no destructor is given.
        EnumRecordHook()
        : first( nullptr )
        {}
}; // EnumRecordHook
} //namespace alib::enumrecords[::detail]

// #################################################################################################
// GetRecord/TryRecord
// #################################################################################################
#include "ALib.Lang.CIFunctions.H"

/// Returns the enum record for element \p{element} of enumeration type \p{TEnum} .
/// In debug-compilations an \alib assertion is raised, if no enum record was defined for
/// \p{element}.
///
/// Internally, references to enum records are stored in a hash map. With that, this method executes
/// in constant time <em>O(1)</em> in the average case.
///
/// \see
/// - Function \alib{enumrecords;TryRecord} which returns a pointer to the record and \c nullptr if
///   no record was defined for \p{element}.
/// - Class \b Enum of the module \alib_boxing and according namespace functions
///    \alib{boxing;GetRecord} and \alib{boxing;TryRecord}, which together allow collecting the
///    necessaryrun-time information to defer the retrieval of enum records.
///
/// @tparam TEnum     The enumeration type. Deduced from given argument.
/// @param  element   The enum element to search for.
/// @return The record that is associated with \p{element}.
template<typename TEnum>
requires alib::enumrecords::HasRecords<TEnum>
const typename RecordsTraits<TEnum>::Type& GetRecord( TEnum element )
{
    static_assert( std::is_trivially_destructible<typename RecordsTraits<TEnum>::Type>::value,
                   "Error: Enum Record types must be a trivially destructible." );

    const void* result= detail::getEnumRecord( typeid(TEnum), integer(element) );
    ALIB_ASSERT_ERROR( result != nullptr, "ENUMS", "Enum Record for type <{}>({}) not found.",
                                          &typeid(TEnum), UnderlyingIntegral(element) )
    return *reinterpret_cast<const typename RecordsTraits<TEnum>::Type*>( result );
}

//==================================================================================================
/// If defined, returns a pointer to the enum record for element \p{element} of enumeration
/// type \p{TEnum} .
/// If no enum record was defined, \c nullptr is returned.
///
/// \see
/// - Function \alib{enumrecords;GetRecord} which returns a reference and asserts in debug-compilations
///   if no record is defined for \p{element} during bootstrap.
/// - Class \b Enum of the module \alib_boxing and the corresponding functions
///   \alib{boxing;GetRecord} and \alib{boxing;TryRecord}, which together allow the collection
///   of the necessary run-time information and defer the retrieval of enum records.
///
/// @tparam TEnum     The enumeration type. Deduced from given argument.
/// @param  element   The enum element to search for.
/// @return A pointer to the record that is associated with \p{element}, respectively \c nullptr
///         if no record was defined.
//==================================================================================================
template<typename TEnum>
requires alib::enumrecords::HasRecords<TEnum>
const typename RecordsTraits<TEnum>::Type* TryRecord( TEnum element )
{
    static_assert( std::is_trivially_destructible<typename RecordsTraits<TEnum>::Type>::value,
                   "Error: Enum Record types must be a trivially destructible." );

    return reinterpret_cast<const typename RecordsTraits<TEnum>::Type*>(
       detail::getEnumRecord( typeid(TEnum), integer(element) ) );
}

#include "ALib.Lang.CIMethods.H"

// #################################################################################################
// struct EnumRecords
// #################################################################################################
//==================================================================================================
/// This is a pure static interface type usable with enumeration types that dispose of a
/// specialization of the type trait \alib{enumrecords;RecordsTraits}.
///
/// The type has two main purposes:
///
/// 1. Providing overloaded methods #Bootstrap which allow to defined enum data records during
///    \ref alib_mod_bs "bootstrapping of ALib" and the using software.
/// 2. Providing an iterator over all records defined for elements of \p{TEnum}.
///
/// A third purpose, namely to retrieve the default (single) enumeration record for a specific enum
/// element is not provided by this type, but instead offered with likewise templated namespace
/// functions
/// - \alib{enumrecords::GetRecord} and
/// - \alib{enumrecords::TryRecord}.
///
/// \note
///   The rationale for this is techical: Using namespace methods, the compiler can deduce
///   template parameter \p{TEnum} from the function parameter, which was not possible if the
///   functions were static methods of this templated type.
///
/// \attention
///   While this type becomes available with the inclusion if the header \implude{EnumRecords},
///   the overloaded \b Bootstrap methods (which are to be used solely during bootstrap, with
///   single-threaded access to this backend), become available only with further inclusion of
///   \implude{EnumRecords.Bootstrap}.
///
/// The rationale for the above is twofold: It reduces header-file dependencies from the majority
/// of code that just use enum records and not defne them. Secondly, it stresses the fact that
/// the variouos \b %Bootstrap methods must be used only while bootstrapping is done.
///
/// \see Chapter \ref alib_enums_records "ALib Enum Records" of the Programmer's Manual of this
///      module.
///
/// @tparam TEnum       The enumeration type that the static methods of this type are to use.
//==================================================================================================
template<typename TEnum>
requires alib::enumrecords::HasRecords<TEnum>
struct EnumRecords
{
    static_assert( std::is_trivially_destructible<typename RecordsTraits<TEnum>::Type>::value,
                   "Error: Enum Record types must be a trivially destructible." );

    /// Defaulted constructor.
    EnumRecords()                                                            noexcept  = default;

    /// The enum's underlying integer type.
    using  TIntegral=  typename std::underlying_type<TEnum>::type;

    /// The enum's associated record type.
    using  TRecord  =  typename RecordsTraits<TEnum>::Type;

    /// Tests if \p{TEnum} has an associated record type that either is or is derived from
    /// type \p{TRecord}.
    /// @tparam TRecord     The enum record base type required.
    /// @tparam TAssociated Deduced by the compiler. Must not be specified.
    /// @return Returns \c true if the given record type is the same or a base class of the
    ///          record type associated with the given enum. Otherwise, returns \c false.
    template<typename TRecord, typename TAssociated= typename RecordsTraits<TEnum>::Type>
    static constexpr bool AreOfType()            { return std::is_base_of_v<TRecord, TAssociated>; }

    /// Implementation of \c std::iterator_traits for enum records.
    /// Begin- and end-iterators can be received with \e static methods
    /// \alib{enumrecords;EnumRecords::begin} and \alib{enumrecords;EnumRecords::end}. In ranged base
    /// <c>for(:)</c> loops, a local instance of type \b EnumRecords has to be created.
    ///
    /// As the name of the class indicates, this iterator satisfies the C++ standard library
    /// concept \https{ForwardIterator,en.cppreference.com/w/cpp/concept/ForwardIterator}.
    struct ForwardIterator
    {
        private:
            #if !DOXYGEN
                friend struct EnumRecords;
            #endif


            /// Shortcut to the record hook's inner node type (linked list).
            using Node= typename detail::EnumRecordHook<TEnum>::Node;

            /// The current node this iterator refers to.
            Node*          node;

            /// Constructor.
            /// @param start Pointer to the initial element.
            ForwardIterator( Node* start )                                                  noexcept
            : node( start )
            {}

            using iterator_category = std::forward_iterator_tag;  ///< Implementation of <c>std::iterator_traits</c>.
            using value_type        = const TRecord&           ;  ///< Implementation of <c>std::iterator_traits</c>.
            using difference_type   = integer                  ;  ///< Implementation of <c>std::iterator_traits</c>.
            using pointer           = const TRecord*           ;  ///< Implementation of <c>std::iterator_traits</c>.
            using reference         = const TRecord&           ;  ///< Implementation of <c>std::iterator_traits</c>.

        public:

            // ######################   To satisfy concept of  InputIterator   ######################

            /// Prefix increment operator.
            /// @return A reference to this object.
            ForwardIterator& operator++()
            {
                node= node->next;
                return *this;
            }

             /// Postfix increment operator.
            /// @return An iterator value that is not increased, yet.
            ForwardIterator operator++(int)
            {
                return ForwardIterator( node->next );
            }

            /// Comparison operator.
            /// @param other  The iterator to compare ourselves to.
            /// @return \c true if this and the given iterator are pointing to the same element,
            ///         \c false otherwise.
            bool operator==(ForwardIterator other)                                             const
            {
                return node == other.node;
            }

            /// Comparison operator.
            /// @param other  The iterator to compare ourselves to.
            /// @return \c true if this and given iterator are not equal, \c false otherwise.
            bool operator!=(ForwardIterator other)                                             const
            {
                return !(*this == other);
            }

            // ######################   Member access   ######################

            /// Returns the enum element of the enum record that this iterator refers to.
            /// @return The enum element the current record is associated to.
            TEnum               Enum()                                                         const
            {
                return TEnum( node->integral );
            }

            /// Returns the underlying integral value of the enum element of the enum record
            /// that this iterator refers to.
            /// @return The integral value of the enum element the current record is associated
            ///         to.
            TIntegral           Integral()                                                     const
            {
                return node->integral;
            }

            /// Returns a constant reference to the enum record this iterator refers to.
            /// @return The current enum record.
            const TRecord&      operator*()                                                    const
            {
                return node->record;
            }

            /// Returns a constant pointer to the enum record this iterator refers to.
            /// @return The current enum record.
            const TRecord*      operator->()                                                   const
            {
                return &node->record;
            }
    }; // inner struct ForwardIterator

    /// Returns an iterator referring to the first enum record defined for type \p{TEnum}.
    ///
    /// \note
    ///   Receiving the list of enum records is internally implemented using class
    ///   \alib{singletons;Singleton} and executes in constant time <em>O(1)</em>, in effect almost
    ///   no time.
    ///
    ///  \note
    ///     Like any other entity in this class, this method is static, apart from a defaulted
    ///     (empty) constructor, which is provided for the sole purpose of allowing
    ///     range-based <em><c>for(:)</c></em> loops.
    ///
    ///  @return An iterator to the first record defined for enumeration type \p{TEnum}.
    static
    ForwardIterator  begin()
    {
        return  ForwardIterator( detail::EnumRecordHook<TEnum>::GetSingleton().first );
    }

    /// Returns an iterator referring to the first element behind the list.
    ///
    /// \see The note documented with sibling method #begin.
    /// @return The end of the list.
    static constexpr
    ForwardIterator  end()
    {
        return  ForwardIterator( nullptr );
    }
}; // struct EnumRecords

// #################################################################################################
// ERSerializable
// #################################################################################################

//==================================================================================================
/// This is a <em>built-in</em> record type that can be used to equip custom enumeration types
/// with \ref alib_enums_records "ALib Enum Records".
///
/// This record has two members, #EnumElementName and #MinimumRecognitionLength which usually
/// determines an element's name in a human-readable format, respectively how many starting
/// characters are to be read to recognize the element when parsed.
///
/// Basic versions of such serialization and de-serialization is implemented with this module
/// and documented with chapter
/// \ref alib_enums_records_details_serialization "4.3.1 Serialization/Deserialization" of the
/// Programmer's Manual of this \alibmod_nl. This functionality is likewise available for
/// enumerations equipped with a custom record type that derives from this type. For this reason,
/// all built-in record types of various \alibmods_nl derive from this type.
///
/// If deserialization is not of importance, a derived type may choose to not parse member
/// #MinimumRecognitionLength from a (resourced) string, but initialize it to fixed \c 0 value.
/// This behavior is for example implemented with record \alib{exceptions;ERException} and various
/// types found in module \alib_cli.
///
/// \see For more information, see:
///   - Concept \alib{enumrecords;IsSerializable}.
///   - Chapter \ref alib_enums_records "ALib Enum Records" and its subsection
///     \ref alib_enums_records_details_serialization "4.3.1 Serialization/Deserialization".
///   - Specializations
///     \alib{strings::APPENDABLES;AppendableTraits<TEnum,TChar,TAllocator>} and
///     \alib{strings::APPENDABLES;AppendableTraits<TBitwiseEnum,TChar,TAllocator>}.
///     which allows write enumeration element names to instances of class
///     \alib{strings;TAString;AString}.
///   - Namespace functions of this module for parsing enum element values:
///     - \alib{enumrecords;Parse}
///     - \alib{enumrecords;ParseBitwise} and
///     - \alib{enumrecords;ParseEnumOrTypeBool}.
//==================================================================================================
struct ERSerializable
{
    /// The name of the enum element.
    String  EnumElementName;

    /// Built-in basic de-serialization functions \alib{enumrecords;Parse}, \alib{enumrecords;ParseBitwise} and
    /// \alib{enumrecords;ParseEnumOrTypeBool} interpret this value as the minimum length (abbreviation)
    /// to accept when an enum element name is parsed.
    /// If \c 0 or negative, the complete #EnumElementName is expected.
    int     MinimumRecognitionLength;


    /// Defaulted constructor leaving the record undefined.
    ERSerializable()                                                            noexcept  = default;

    /// Constructor. This is either called by descendants or by user code that omits the preferred
    /// option of parsing resourced strings for the creation of enum records.
    ///
    /// Note that parameter \p{name} has to be of "static nature", which means the buffer and
    /// contents of the string is deemed to survive the life-cycle of an application.
    /// With direct invocation, usually, C++ string literals are passed.
    ///
    ///  @param name      The name of this element.
    ///  @param minLength The minimum length to recognise an element when de-serialized.
    ///                   (Assigned to field #MinimumRecognitionLength.)
    ERSerializable( const String& name, int minLength= 0 )                                  noexcept
    : EnumElementName         (name)
    , MinimumRecognitionLength(minLength)
    {}

    /// Parses the fields of this record from the \b Substring \p{parser} given as reference.
    /// In case of an error, an \alib exception is raised, as parsing has to succeed.
    ///
    /// @see
    ///   This is the implementation of a method needed with template programming, as described in
    ///   \alib{enumrecords;EnumRecordPrototype::Parse}.
    ALIB_DLL
    void Parse();
};

ALIB_WARNINGS_IGNORE_DOCS

/// Concept that is satisfied if the type trait \alib{enumrecords;RecordsTraits}
/// is specialized for type \p{TEnum} and its type-alias member \b Type evaluates to
/// \alib{enumrecords;ERSerializable} or a derived type thereof.
/// @tparam TEnum The type to test.
template<typename TEnum>
concept IsSerializable = std::is_base_of_v<ERSerializable, typename RecordsTraits<TEnum>::Type>;

ALIB_WARNINGS_RESTORE


} // namespace alib[::enumrecords]

/// Type alias in namespace \b alib.
template<typename TEnum>
using     EnumRecords=          enumrecords::EnumRecords<TEnum>;

} // namespace [alib]



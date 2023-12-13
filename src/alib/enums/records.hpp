/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_enums of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_ENUMS_RECORDS
#define HPP_ALIB_ENUMS_RECORDS 1

#if !defined(HPP_ALIB_SINGLETONS_SINGLETON)
#   include "alib/singletons/singleton.hpp"
#endif

ALIB_ASSERT_MODULE(ENUMS)

#if !defined (HPP_ALIB_ENUMS_UNDERLYING_INTEGRAL)
#    include "alib/enums/underlyingintegral.hpp"
#endif

#if ALIB_STRINGS && !defined (HPP_ALIB_STRINGS_STRING)
#    include "alib/strings/string.hpp"
#endif

#if !defined (HPP_ALIB_TOOLS)
#   include "alib/lib/tools.hpp"
#endif

#if ALIB_DEBUG
#   if !defined (HPP_ALIB_TOOLS)
#      include "alib/lib/tools.hpp"
#   endif
#   if ALIB_STRINGS && !defined(HPP_ALIB_STRINGS_LOCALSTRING)
#       include "alib/strings/localstring.hpp"
#   endif
#endif


#if !defined(_GLIBCXX_UTILITY) || !defined(_UTILITY_)
#   include <utility>
#endif

#if !defined (_GLIBCXX_ITERATOR) && !defined (_ITERATOR_)
#   include <iterator>
#endif


namespace aworx { namespace lib {

// forwards
#if ALIB_FILESET_MODULES
    class Module;
#endif

#if ALIB_RESOURCES
namespace resources { class ResourcePool; }
#endif

namespace enums {


// #################################################################################################
// enums::Bootstrap()
// #################################################################################################

#if defined(ALIB_DOX)
            /** ************************************************************************************************
             * This method initializes enum records, of built-in types i.e. those of the enumerations found
             * in header file \alibheader{lib/fs_commonenums/commonenums.hpp}.
             *
             * This method needs to be called with bootstrapping a software.
             * The \ref alib_manual_bootstrapping "standard bootstrap" code of \alib will perform this.
             * Only if fileset \alibfs_modules is not included in the \alibdist_nl, this
             * function has to be invoked "manually".
             *
             * \note
             *   In fact, if fileset \alibfs_modules is included, then this function is empty, because
             *   the enumeration records will in this case be resourced in singleton \ref aworx::ALIB of type
             *   \alib{ALibDistribution}.
             *
             * Multiple invocations of this method are ignored.
             *
             * \see
             *   For information about using this method, consult chapter
             *   \ref alib_manual_bootstrapping_smallmods of the \ref alib_manual.
             * ************************************************************************************************/
            ALIB_API
            void        Bootstrap();
#elif !ALIB_FILESET_MODULES
            ALIB_API void   Bootstrap();
#else

            inline void Bootstrap()
            {}

#endif // !ALIB_FILESET_MODULES


// #################################################################################################
// T_EnumRecords
// #################################################################################################

#if defined(ALIB_DOX)
            /** ************************************************************************************************
             * This TMP struct is used to associate an <b><em>"ALib Enum Record"</em></b> type to a
             * (non-scoped or C++ 11 scoped) enumeration type.
             *
             * In the non-specialized version type definition #Type (the only entity of this struct)
             * evaluates to <c>void</c>. To assign a record, specify a data record type, similar to what
             * is prototyped with \alib{enums,EnumRecordPrototype}
             *
             * \see
             *   Please consult chapter \ref alib_enums_records "4. Enum Records" of the Programmer's Manual
             *   of \alib_enums for detailed documentation and sample code on this struct and underlying
             *   concept.
             *
             * \see
             *   Macro \ref ALIB_ENUMS_ASSIGN_RECORD offers a well readable alternative to specialize this
             *   struct for an enum type.
             *
             * @tparam TEnum      The enumeration type this struct applies to.
             * @tparam TEnableIf  Internally used to ensure that given type \p{TEnum} is an enumeration.
             *                    Defaulted. Please do not specify!
             **************************************************************************************************/
            template<typename TEnum, typename TEnableIf>
            struct T_EnumRecords
            {
                /** The data type associated with elements of \p{TEnum}.   */
                using  Type=   void;
            };
#else
            template<typename TEnum,
                typename TEnableIf= ATMP_VOID_IF( std::is_enum<TEnum>::value )>
            struct T_EnumRecords
            {
                using Type=   void;
            };
#endif

// #################################################################################################
// detail {}
// #################################################################################################
}}}
#include "alib/enums/detail/enumrecords.inl"
namespace aworx { namespace lib { namespace enums {



// #################################################################################################
// GetRecord/TryRecord
// #################################################################################################

#if defined(ALIB_DOX)
/** ************************************************************************************************
 * Returns the enum record for element \p{element} of enumeration type \p{TEnum} .
 * In debug-compilations an \alib assertion is raised, if no enum record was defined for
 * \p{element}.
 *
 * Internally, references to enum records are stored in a hash map. With that, this method executes
 * in constant time <em>O(1)</em> in the average case.
 *
 * \see
 * - Function \alib{enums,TryRecord} which returns a pointer to the record and \c nullptr if
 *   no record was defined for \p{element}.
 * - Class \b Enum of module \alib_boxing and its method \alib{boxing,Enum::GetRecord},
 *   which allows to collect the necessary run-time information to defer the retrieval of
 *   enum records.
 *
 * @tparam TEnum     The enumeration type. Deduced from given argument.
 * @param  element   The enum element to search for.
 * @return The record that is associated with \p{element}.
 **************************************************************************************************/
template<typename TEnum>
inline
const typename T_EnumRecords<TEnum>::Type&  GetRecord( TEnum element );
#else
template<typename TEnum>
inline
ATMP_T_IF( const typename T_EnumRecords<TEnum>::Type&,
           !ATMP_EQ(typename T_EnumRecords<TEnum>::Type, void) )
GetRecord( TEnum element )
{
    static_assert( std::is_trivially_destructible<typename T_EnumRecords<TEnum>::Type>::value,
                   "Error: Enum Record types must be a trivially destructible." );

    const void* result= detail::getEnumRecord( typeid(TEnum),
                                               static_cast<integer>( UnderlyingIntegral(element) ) );
    #if ALIB_STRINGS
        ALIB_ASSERT_ERROR( result != nullptr, "ENUMS",
                           NString128() << "Enum Record for type <" << DbgTypeDemangler( typeid(TEnum)).Get()
                                        << ">(" << UnderlyingIntegral(element)
                                        << ") not found." )
    #else
        ALIB_ASSERT_ERROR( result != nullptr, "ENUMS: Enum Record for not found." )
    #endif
    return *reinterpret_cast<const typename T_EnumRecords<TEnum>::Type*>( result );
}
#endif

#if defined(ALIB_DOX)
/** ************************************************************************************************
 * If defined, returns a pointer to the enum record for element \p{element} of enumeration
 * type \p{TEnum} .
 * If no enum record was defined, \c nullptr is returned.
 *
 * \see
 * - Function \alib{enums,GetRecord} which returns a reference and asserts in debug-compilations
 *   if no record is defined for \p{element} during bootstrap.
 * - Class \b Enum of module \alib_boxing and its method \alib{boxing,Enum::TryRecord},
 *   which allows to collect the necessary run-time information to defer the retrieval of
 *   enum records.
 *
 * @tparam TEnum     The enumeration type. Deduced from given argument.
 * @param  element   The enum element to search for.
 * @return A pointer to the record that is associated with \p{element}, respectively \c nullptr
 *         if no record was defined.
 **************************************************************************************************/
template<typename TEnum>
inline
const typename T_EnumRecords<TEnum>::Type*   TryRecord( TEnum element );
#else
template<typename TEnum>
inline
ATMP_T_IF( const typename T_EnumRecords<TEnum>::Type*, !ATMP_EQ(typename T_EnumRecords<TEnum>::Type, void) )
TryRecord( TEnum element )
{
    static_assert( std::is_trivially_destructible<typename T_EnumRecords<TEnum>::Type>::value,
                   "Error: Enum Record types must be a trivially destructible." );

    return reinterpret_cast<const typename T_EnumRecords<TEnum>::Type*>(
       detail::getEnumRecord( typeid(TEnum),  static_cast<integer>( UnderlyingIntegral(element) ) ));
}
#endif



// #################################################################################################
// struct EnumRecords
// #################################################################################################
#if defined(ALIB_DOX)
/** ************************************************************************************************
 * This is a pure static interface type usable with enumeration types that dispose about a
 * specialization of TMP struct \alib{enums,T_EnumRecords}.
 *
 * The type has two main purposes:
 *
 * 1. Providing overloaded methods #Bootstrap which allow to defined enum data records during
 *    \ref alib_manual_bootstrapping "bootstrapping of ALib" and the using software.
 * 2. Providing an iterator over all records defined for elements of \p{TEnum}.
 *
 * A third purpose, namely to retrieve the default (single) enumeration record for a specific enum
 * element is not provided by this type, but instead offered with likewise templated namespace
 * functions
 * - \alib{enums::GetRecord} and
 * - \alib{enums::TryRecord}.
 *
 * \note
 *   The rational for this is techical: Using namespace methods, the compiler can deduce
 *   template parameter \p{TEnum} from the function parameter, which was not possible if the
 *   functions were static methods of this templated type.
 *
 * \attention
 *   While this type becomes available with the inclusion of header file
 *   \alibheader{enums/records.hpp}, the overloaded \b Bootstrap methods, which are to be used
 *   solely during bootstrap, with single-threaded access to this backend, become available
 *   only with further inclusion of header \alibheader{enums/recordbootstrap.hpp}, which provides
 *   these method's defintion.
 *
 * The rational for the above is twofold: It reduces header file dependcies from the majority
 * of code that just use enum records and not defne them. Secondly it stresses the fact that
 * the #Bootstrap methods must be used only while bootstrapping is done.
 *
 * \see Chapter \ref alib_enums_records "ALib Enum Records" of the Programmer's Manual of this
 *      module.
 *
 * @tparam TEnum       The enumeration type that the static methods of this type are to use.
 * @tparam TEnableIf   To be ignored. Used to select this struct only for enum types which have
 *                     a specialization of \alib{enums,T_EnumRecords} in place.
 **************************************************************************************************/
template<typename TEnum, typename TEnableIf>
struct EnumRecords
#else
template<typename TEnum, typename TEnableIf=
ATMP_VOID_IF( !ATMP_EQ(typename T_EnumRecords<TEnum>::Type, void) ) >
struct EnumRecords
#endif // defined(ALIB_DOX)
{
    static_assert( std::is_trivially_destructible<typename T_EnumRecords<TEnum>::Type>::value,
                   "Error: Enum Record types must be a trivially destructible." );

    /** Defaulted constructor.   */
    EnumRecords()                                                            noexcept  = default;

    /** The enum's underlying integer type. */
    using  TIntegral=  typename std::underlying_type<TEnum>::type;

    /** The enum's associated record type. */
    using  TRecord  =  typename T_EnumRecords<TEnum>::Type;

    #if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Tests if \p{TEnum} has an associated record type that either is or is derived from
     * type \p{TRecord}.
     * @tparam TRecord     The enum record base type required.
     * @tparam TAssociated Deduced by the compiler. Must not be specified.
     * @return Returns \c true if the given record type is the same or a base class of the
     *          record type associated with the given enum. Otherwise returns \c false.
     **********************************************************************************************/
    template<typename TRecord, typename TAssociated>
    static  constexpr
    bool AreOfType();
    #else
    template<typename TRecord, typename TAssociated= typename T_EnumRecords<TEnum>::Type>
    static constexpr
    bool AreOfType()
    {
        return ATMP_ISOF( TAssociated, TRecord );
    }
    #endif

    /** ****************************************************************************************
     * Implementation of \c std::iterator_traits for enum records.
     * Begin- and end-iterators can be received with \e static methods
     * \alib{enums,EnumRecords::begin} and \alib{enums,EnumRecords::end}. In ranged base
     * <c>for(:)</c> loops, a local instance of type \b EnumRecords has to be created.
     *
     * As the name of the class indicates, this iterator satisfies the C++ standard library
     * concept \https{ForwardIterator,en.cppreference.com/w/cpp/concept/ForwardIterator}.
     ******************************************************************************************/
    struct ForwardIterator
    {
        private:
            #if !defined(ALIB_DOX)
                friend struct EnumRecords;
            #endif


            /** Shortcut to the record hook's inner node type (linked list). */
            using Node= typename detail::EnumRecordHook<TEnum>::Node;

            /** The current node this iterator refers to. */
            Node*          node;

            /** Constructor.
             *  @param start Pointer to the initial element.   */
            ForwardIterator( Node* start )                                                  noexcept
            : node( start )
            {}

            using iterator_category = std::forward_iterator_tag;  ///< Implementation of <c>std::iterator_traits</c>.
            using value_type        = const TRecord&           ;  ///< Implementation of <c>std::iterator_traits</c>.
            using difference_type   = integer                  ;  ///< Implementation of <c>std::iterator_traits</c>.
            using pointer           = TRecord const*           ;  ///< Implementation of <c>std::iterator_traits</c>.
            using reference         = const TRecord&           ;  ///< Implementation of <c>std::iterator_traits</c>.

        public:

            // ######################   To satisfy concept of  InputIterator   ######################

            /** Prefix increment operator.
             *  @return A reference to this object. */
            ForwardIterator& operator++()
            {
                node= node->next;
                return *this;
            }

             /** Postfix increment operator.
             *  @return An iterator value that is not increased, yet. */
            ForwardIterator operator++(int)
            {
                return ForwardIterator( node->next );
            }

            /** Comparison operator.
             *  @param other  The iterator to compare ourselves to.
             *  @return \c true if this and the given iterator are pointing to the same element,
             *          \c false otherwise. */
            bool operator==(ForwardIterator other)                                             const
            {
                return node == other.node;
            }

            /** Comparison operator.
             *  @param other  The iterator to compare ourselves to.
             *  @return \c true if this and given iterator are not equal, \c false otherwise. */
            bool operator!=(ForwardIterator other)                                             const
            {
                return !(*this == other);
            }

            // ######################   Member access   ######################

            /** Returns the enum element of the enum record that this iterator refers to.
             *  @return The enum element the current record is associated to.               */
            TEnum               Enum()                                                         const
            {
                return TEnum( node->integral );
            }

            /** Returns the underlying integral value of the enum element of the enum record
             *  that this iterator refers to.
             *  @return The integral value of the enum element the current record is associated
             *          to.                                                                     */
            TIntegral           Integral()                                                     const
            {
                return node->integral;
            }

            /** Returns a constant reference to the enum record this iterator refers to.
             *  @return The current enum record.                                            */
            const TRecord&      operator*()                                                    const
            {
                return node->record;
            }

            /** Returns a constant pointer to the enum record this iterator refers to.
             *  @return The current enum record.                                            */
            TRecord const*      operator->()                                                   const
            {
                return &node->record;
            }
    }; // inner struct ForwardIterator

    /**
     * Returns an iterator referring to the first enum record defined for type \p{TEnum}.
     *
     * \note
     *   Receiving the list of enum records is internally implemented using class
     *   \alib{singletons,Singleton} and executes in constant time <em>O(1)</em>, in effect almost
     *   no time.
     *
     *  \note
     *     Like any other entity in this class, this method is static, apart from a defaulted
     *     (empty) constructor, which is provided for the sole purpose of allowing C++ 11
     *     range-based <em><c>for(:)</c></em> loops.
     *
     *  @return An iterator to the first record defined for enumeration type \p{TEnum}.
     */
    static
    ForwardIterator  begin()
    {
        return  ForwardIterator( detail::EnumRecordHook<TEnum>::GetSingleton().first );
    }

    /**
     * Returns an iterator referring to the first element behind the list.
     *
     * \see The note documented with sibling method #begin.
     * @return The end of the list.                                                       */
    static constexpr
    ForwardIterator  end()
    {
        return  ForwardIterator( nullptr );
    }


    // #############################################################################################
    // Record initialization
    // Method implementations are given in separated header file
    // #############################################################################################

    /**
     * Helper struct used with bulk-initialization method #Bootstrap(std::initializer_list<Initializer>).
     */
    struct Initializer
    {
        /** The enumeration element. */
        TEnum   element;

        /** The static data record. */
        TRecord record;

        /**
         * Constructor taking variadic template arguments to construct the record.
         *
         * @tparam TArgs  Types of the variadic arguments \p{args}.
         * @param  elem   The enum element.
         * @param  args   Variadic arguments forwarded to constructor of field #record.
         */
        template <typename... TArgs>
        Initializer( TEnum elem, TArgs&&... args)                      noexcept
        : element( elem                         )
        , record ( std::forward<TArgs>(args)... )
        {}
    };

    /**
     * Defines a record for a single element of \p{TEnum}.
     *
     * \note
     *   This method is rather provided for completeness, than for actual use, because it is
     *   preferred to bootstrap enum records as "bulk" data.
     *   Furthermore, it is preferred to use overloaded versions that accept static string data
     *   used to parse the data from. This is more efficient in respect to the footprint of
     *   an application, and - if strings are \ref alib_mod_resources "resourced" -
     *   far more flexible.
     *
     * \see Chapter \ref alib_enums_records_firststep_init for a sample of how this method
     *      can be invoked.
     *
     * @tparam TArgs   Types of the variadic arguments \p{args}.
     * @param  element The enum element.
     * @param  args    Variadic arguments forwarded to constructor of the custom record to
     *                 create and store.
     */
    template <typename... TArgs>
    static inline
    void Bootstrap( TEnum element, TArgs&&... args )                                 noexcept;

    /**
     * Associates elements of \p{TEnum} with records, as specified by the given list of
     * \p{definitions}.
     *
     * The use of inner struct \alib{enums::EnumRecords,Initializer} allows to place
     * the enumeration element together with the construction parameters of the custom record
     * type into one comma-separated argument list, without the need to place extra curly braces
     * around the arguments of the record. (Such would have been necessary if for example
     * <c>std::pair</c> had been used).
     *
     * \note
     *   It is preferred to use overloaded versions that parse definitions from
     *   static string data. This is more efficient in respect to the footprint of
     *   an application, and - if strings are \ref alib_mod_resources "resourced" -
     *   far more flexible.
     *
     * \see Chapter \ref alib_enums_records_firststep_init for a sample of how this method
     *      can be invoked.
     *
     * @param definitions List of static enum records to store.
     */
    static inline
    void Bootstrap( std::initializer_list<Initializer> definitions );


    #if ALIB_STRINGS

        /**
         * Reads a list of enum data records from given string \p{input}.
         *
         * The contents (buffer) of the given substring has to be of static nature (by contract).
         * This means that parsing will not create copies of portions of the string but still
         * use them later. Consequently the given string's buffer has to survive the life-cycle
         * of an application.
         *
         * This is due to the static nature of \ref alib_enums_records "ALib Enum Records" and their
         * creation during bootstrap, either from C++ string literals or
         * \ref alib_mod_resources "ALib Externalized Resources", which comply to the same contract.
         *
         * ### Availability ###
         * This method is available only if \alib_strings is included in the \alibdist.
         *
         * \see Chapter \ref alib_enums_records_resourced_parsing for a sample of how this method
         *      can be invoked.
         *
         * @param input      The string used for parsing the enum records to store.
         * @param innerDelim The delimiter used for separating the fields of a record.
         *                   Defaults to <c>','</c>.
         * @param outerDelim The character delimiting enum records.
         *                   Defaults to <c>','</c>.
         */
        static inline
        void Bootstrap( const String& input, character innerDelim=',', character outerDelim= ',' );
    #endif

    #if ALIB_RESOURCES
        /**
         * Reads a list of enum data records from an (externalized) resource string.
         *
         * It is possible to provide the record data in two ways:
         * - In one resource string: In this case, parameter \p{outerDelim} has to specify
         *   the delimiter that separates the records.
         * - In an array of resource strings: If the resource string as given is not defined, this
         *   method appends an integral index starting with \c 0 to the resource name, parses
         *   a single record and increments the index.
         *   Parsing ends when a resource with a next higher index is not found.
         *
         * The second option is recommended for larger enum sets. While the separation causes
         * some overhead in a resource backend, the external (!) management (translation,
         * manipulation, etc.) is most probably simplified with this approach.
         *
         * ### Availability ###
         * This method is available only if \alib_resources is included in the \alibdist.
         *
         * \see Chapter \ref alib_enums_records_resourced for a sample of how this method
         *      can be invoked.
         *
         * @param pool       The resource pool to receive the string to parse the records from.
         * @param category   The resource category of the externalized string.
         * @param name       The resource name of the externalized name. In the case that a
         *                   resource with that name does not exist, it is tried to load
         *                   a resource with index number \c 0 appended to this name, aiming to
         *                   parse a single record. On success, the index is incremented until
         *                   no consecutive resource is found.
         * @param innerDelim The delimiter used for separating the fields of a record.
         *                   Defaults to <c>','</c>.
         * @param outerDelim The character delimiting enum records.
         *                   Defaults to <c>','</c>.
         */
        static
        inline
        void Bootstrap( resources::ResourcePool& pool, const NString& category, const NString& name,
                        character innerDelim= ',', character outerDelim= ','                 );

        /**
         * This method is available if TMP struct \alib{resources,T_Resourced} is specialized for
         * enum type \p{TEnum}.<br>
         * Invokes #Bootstrap(ResourcePool&, const NString&, const NString&, character, character)
         *
         *
         * ### Availability ###
         * This method is available only if \alib_resources is included in the \alibdist.
         *
         * \see Chapter \ref alib_enums_records_resourced_tresourced of the Programmer's Manual
         *      of this module.
         *
         * @param innerDelim The delimiter used for separating the fields of a record.
         *                   Defaults to <c>','</c>.
         * @param outerDelim The character delimiting enum records.
         *                   Defaults to <c>','</c>.
         *
         */
        static inline
        void Bootstrap( character innerDelim=',', character outerDelim= ',' );

        #if ALIB_FILESET_MODULES
            /**
             * This method can be used if a set of enum records is resourced using an \alib
             * \alib{Module}'s resource instance.
             *
             * Invokes #Bootstrap(ResourcePool&, const NString&, const NString&, character, character)
             * accepting a \alib{Module} and using its \alib{resources,ResourcePool} and
             * field \alib{Module::ResourceCategory}.
             *
             * \note
             *   This is the preferred overload taken with \alib to load built-in enum records.
             *   The only exclamation is the use of overload #Bootstrap(character, character)
             *   for enum record types that require a specialization of \alib{resources,T_Resourced}
             *   to perform "recursive" acquisition of other resources defined by fields of the
             *   records.
             *
             * ## Availability ##
             * This method is available only if
             * \ref alib_manual_modules_filesets "fileset \"Modules\"" is included in the \alibdist.
             *
             * \see Chapters \ref alib_enums_records_resourced_tresourced and
             *      \ref alib_enums_records_resourced_from_modules for more information.
             *
             * @param module     The module to use the resource pool and category name from.
             * @param name       The resource name of the externalized name. In the case that a
             *                   resource with that name does not exist, it is tried to load
             *                   a resource with index number \c 0 appended to this name, aiming to
             *                   parse a single record. On success, the index is incremented until
             *                   no consecutive resource is found.
             * @param innerDelim The delimiter used for separating the fields of a record.
             *                   Defaults to <c>','</c>.
             * @param outerDelim The character delimiting enum records.
             *                   Defaults to <c>','</c>.
             */
            static inline
            void Bootstrap( Module&    module          , const NString& name,
                            character  innerDelim= ',' , character      outerDelim= ',' );
        #endif
    #endif

}; // struct EnumRecords

// #################################################################################################
// ERSerializable
// #################################################################################################

#if ALIB_STRINGS
/** ************************************************************************************************
 * This is a <em>built-in</em> record type that can be used to equip custom enumeration types
 * with \ref alib_enums_records "ALib Enum Records".
 *
 * This record has two members, #EnumElementName and #MinimumRecognitionLength which usually
 * determines an element's name in a human readable format, respectively how many starting
 * characters are to be read to recognize the element when parsed.
 *
 * Basic versions of such serialization and de-serialization is implemented with this module
 * and documented with chapter \ref alib_enums_records_details_serialization of the Programmer's Manual
 * of this \alibmod_nl. This functionality is likewise available for enumerations equipped with
 * a custom record type that derives from this type. For this reason, all built-in
 * record types of various \alibmods_nl derive from this type.
 *
 * If deserialization is not of importance, a derived type may choose to not parse member
 * #MinimumRecognitionLength from a (resourced) string, but initialize it to fixed \c 0 value.
 * This behavior is for example implemented with record \alib{results,ERException} and various
 * types found in module \alib_cli.
 *
 * \see For more information see:
 *   - Chapter \ref alib_enums_records "ALib Enum Records" and its sub-section
 *     \ref alib_enums_records_details_serialization.
 *   - Specializations
 *     \alib{strings::APPENDABLES,T_Append<TEnum\,TChar>,T_Append<TEnum\,TChar>} and
 *     \alib{strings::APPENDABLES,T_Append<TEnumBitwise\,TChar>,T_Append<TEnumBitwise\,TChar>}.
 *     which allows write enumeration element names to instances of class
 *     \alib{strings,TAString,AString}.
 *   - Namespace functions of this module for parsing enum element values:
 *     - \alib{enums,Parse}
 *     - \alib{enums,ParseBitwise} and
 *     - \alib{enums,ParseEnumOrTypeBool}.
 **************************************************************************************************/
struct ERSerializable
{
    /** The name of the enum element.         */
    String  EnumElementName;

    /**
     * Built-in basic de-serialization functions \alib{enums,Parse}, \alib{enums,ParseBitwise} and
     * \alib{enums,ParseEnumOrTypeBool} interpret this value as the minimum length (abbreviation)
     * to accept when an enum element name is parsed.
     * If \c 0 or negative, the complete #EnumElementName is expected.
     */
    int     MinimumRecognitionLength;


    /** Defaulted constructor leaving the record undefined. */
    ERSerializable()                                                                   noexcept  = default;

    /**
     * Constructor. This is either called by descendants or by user code that omits the preferred
     * option of parsing resourced strings for the creation of enum records.
     *
     * Note that parameter \p{name} has to be of "static nature", which means the buffer and
     * contents of the string is deemed to survive the life-cycle of an application.
     * With direct invocation, usually, C++ string literals are passed.
     *
     *  @param name      The name of this element.
     *  @param minLength The minimum length to recognise an element when de-serialized.
     *                   (Assigned to field #MinimumRecognitionLength.)
     */
    ERSerializable( const String& name, int minLength= 0 )                              noexcept
    : EnumElementName         (name)
    , MinimumRecognitionLength(minLength)
    {}

    /**
     * Parses the fields of this record from the \b Substring \p{parser} given as reference.
     * In case of error an \alib exception is raised, as parsing has to succeed.
     *
     * @see
     *   This is the implementation of a method needed with TMP, as described in
     *   \alib{enums,EnumRecordPrototype::Parse}.
     */
    ALIB_API
    void Parse();
};
#endif // ALIB_STRINGS



}} // namespace aworx[::lib::enums]

/// Type alias in namespace #aworx.
template<typename TEnum>
using     T_EnumRecords=        lib::enums::T_EnumRecords<TEnum>;

/// Type alias in namespace #aworx.
template<typename TEnum>
using     EnumRecords=          lib::enums::EnumRecords<TEnum>;

} // namespace [aworx]

// #################################################################################################
// Helper Macros
// #################################################################################################
#define  ALIB_ENUMS_ASSIGN_RECORD( TEnum, TRecord )                                                \
namespace aworx { namespace lib { namespace enums {                                                \
template<> struct T_EnumRecords<TEnum>  :  public std::true_type                                   \
{                                                                                                  \
    using  Type=  TRecord;                                                                         \
};}}}

#endif // HPP_ALIB_ENUMS_RECORDS

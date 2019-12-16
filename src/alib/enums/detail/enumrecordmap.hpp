/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_enums of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_ENUMS_DETAIL_ENUMRECORDMAP
#define HPP_ALIB_ENUMS_DETAIL_ENUMRECORDMAP 1


#if !defined(HPP_ALIB_ENUMS_RECORDS)
#   include "alib/enums/records.hpp"
#endif

#if ALIB_MONOMEM
#   if !defined(HPP_ALIB_MONOMEM_HASHMAP)
#       include "alib/monomem/hashmap.hpp"
#   endif
#else
#   if !defined(_GLIBCXX_UNORDERED_MAP) && !defined(_UNORDERED_MAP_)
#       include <unordered_map>
#   endif
#endif


namespace aworx { namespace lib { namespace enums { namespace detail {


/** ************************************************************************************************
 * The key-type of the central hash table that stores all enum records.
 * Contains inner functors for hashing and comparing.
 **************************************************************************************************/
struct EnumRecordKey
{
    /** Run-time type information on the enumeration type */
    const std::type_info&   RTTI;

    /** Integral value of the enumeration element. */
    integer                 Element;

    /**
     * Constructor.
     * @param rtti     Assigned to #RTTI.
     * @param element  Assigned to #Element.
     */
    EnumRecordKey(const std::type_info& rtti, integer element)
    : RTTI   (rtti)
    , Element(element)
    {}

    /** Hash functor for this key type. */
    struct Hash
    {
        /** Calculate hash code.
         *  @param key  The key to calculate a hash value for.
         *  @return A hash code for this key. */
        std::size_t operator()( const EnumRecordKey & key ) const
        {
            size_t result=    key.RTTI.hash_code()
                            ^ static_cast<size_t>(key.Element);
            #if ALIB_SIZEOF_INTEGER == 4
                result^= (result >> 17);
            #else
                result^= (result >> 33);
            #endif
            return result;
        }
    };

    /** Compare functor for this key type. */
    struct EqualTo
    {
        /**
         * Compares two keys.
         * @param lhs  The left-hand side value.
         * @param rhs  The right-hand side value.
         * @return \c true if the two keys are equal, \c false otherwise.
         */
        bool operator()( const EnumRecordKey & lhs,
                         const EnumRecordKey & rhs ) const
        {
            return    lhs.RTTI    == rhs.RTTI
                   && lhs.Element == rhs.Element;
        }
    };
}; // EnumRecordKey


#if ALIB_MONOMEM
/** ************************************************************************************************
 * This detail function returns \e the central hash table that stores all enum records of all types.
 * A constant reference is returned, which is in alignment with the general contract of this
 * concept that considers enum records as static data which must not be modified after
 * bootstrapping a software.
 *
 * Consequently, there are no mechanisms to protect parallel access, as after bootstrap exclusively
 * read operations are allowed.
 *
 * Prior to bootstrapping, the returned map may be <c>const_cast</c>'ed to a mutable reference
 * for the sake of modifying the base- and maximum load factors as well as for reserving
 * a certain element capacity. The default values are already raised to \c 3.0 and \c 6.0,
 * as the retrieval of enumeration records is not considered to be done in time-critical
 * code units.
 *
 * Direct access to this map is not recommended, and useful only in seldom cases.
 *
 * A good sample for its use, is to provide debug-output of all defined records for a type in the
 * case that a record for a certain enumeration element of that type was not found, and such
 * output is required to be performed in a code unit that has lost (templated) compile-time type
 * information.
 *
 * \note
 *   Run-time access to single records is provided with function
 *   \alib{enums,detail::getEnumRecord(const std::type_info& rtti, integer)} and encapsulated
 *   in type \alib{boxing,Enum} of module \alib_boxing.
 *
 * \attention
 *   In the absence of module \alib_monomem in an \alibdist, the returned type changes to
 *   <c>std::unordered_map</c>.
 *
 * @return A constant reference to the internal hash map storing all enum records.
 **************************************************************************************************/
const HashMap           < EnumRecordKey, const void*,
                          EnumRecordKey::Hash,
                          EnumRecordKey::EqualTo       >&  getInternalRecordMap();
#else
const std::unordered_map< EnumRecordKey, const void*,
                          EnumRecordKey::Hash,
                          EnumRecordKey::EqualTo       >&  getInternalRecordMap();
#endif


}}}}   // namespace [aworx::lib::enums::detail]

#endif // HPP_ALIB_ENUMS_DETAIL_ENUMRECORDMAP

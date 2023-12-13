/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_enums of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_ENUMS_DETAIL_ENUMRECORDS
#define HPP_ALIB_ENUMS_DETAIL_ENUMRECORDS 1

#if !defined(HPP_ALIB_ENUMS_RECORDS)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if !defined(HPP_ALIB_INTEGERS)
#   include "alib/lib/integers.hpp"
#endif


namespace aworx { namespace lib { namespace enums {

/**
 * Details of namespace #aworx::lib::enums.
 */
namespace detail {

// #################################################################################################
// Detail functions
// #################################################################################################
/** ************************************************************************************************
 * Stores the \p{record} for enum \p{element} of the given enum type \p{rtti}.
 * If a value already exists, it is ignored. This allows to have multiple records, while only
 * the first is found.
 * @param rtti     The enumeration type.
 * @param integral The integral value of the enum element.
 * @param record   The record to associate.
 */
ALIB_API
void        setEnumRecord( const std::type_info& rtti, integer integral, const void* record );

/** ************************************************************************************************
 * Retrieves the enum record stored for the enum element with integral value \p{integral} of
 * enum type \p{rtti}.
 * @param rtti     The enumeration type.
 * @param integral The integral value of the enum element.
 * @return A pointer to the record, \c nullptr if not found.
 **************************************************************************************************/
ALIB_API
const void* getEnumRecord( const std::type_info& rtti, integer integral );


/** ************************************************************************************************
 * This is the internal singleton that provides a link to the first
 * \ref alib_enums_records "ALib Enum Record".
 * The class inherit's \alib{singletons,Singleton} and has a private default constructor, what
 * makes this type a \ref alib_singleton_strict "strict singleton".
 *
 * Because enum record types are ensured to be trivially destructible types, no destructor is given.
 *
 * @tparam TEnum  The enumeration type associated with enum records.
 **************************************************************************************************/
template<typename TEnum>
struct EnumRecordHook : public    Singleton<EnumRecordHook<TEnum>>
{
    #if !defined(ALIB_DOX)
        friend aworx::Singleton<EnumRecordHook>;
    #endif

    /** The enum's underlying integer type. */
    using  TIntegral=  typename std::underlying_type<TEnum>::type;

    /** The enum's associated record type. */
    using  TRecord  =  typename        T_EnumRecords<TEnum>::Type;


    /** A node of the forward list that contains the custom record data */
    struct Node
    {
        /** The enum element's underlying integral value. */
        TIntegral   integral;

        /** The data record. */
        TRecord     record;

        /** Pointer to the next node. */
        Node*       next;

        /**
         * Constructs this instance taking variadic template arguments to construct the contained
         * enum record of custom type.
         *
         * @tparam TArgs   Types of the variadic arguments \p{args}.
         * @param  element The enum element
         * @param  args    Variadic arguments forwarded to constructor of field #record.
         */
        template <typename... TArgs>
        Node( TEnum element, TArgs&&... args)                                   noexcept
        : integral( UnderlyingIntegral( element)          )
        , record  ( std::forward<TArgs>(args)... )
        {}

        /** Default constructor. */
        Node()                                                                  noexcept   =default;

    };

    /** The hook to the first record defined. */
    Node*    first;

    /**
     * Helper methods that returns the address of field
     * \alib{enums::detail,EnumRecordHook::Node::next} the last element contained in the
     * list. If no elements have been initialized, yet, the address of field #first is
     * returned.
     * @return  A pointer to the pointer of the last element or this hook.
     */
    Node** getPointerToLast()
    {
        auto** last= &first;
        while( (*last) != nullptr )
            last= &(*last)->next;
        return last;
    }

    private:
        /**
         * Private default constructor, what makes this type a
         * \ref alib_singleton_strict "strict singleton".
         *
         *  \note As enum record types are trivially destructible types, no destructor is given.
         */
        EnumRecordHook()
        : first( nullptr )
        {}

};

}}}}   // namespace [aworx::lib::enums::detail]


#endif // HPP_ALIB_ENUMS_DETAIL_ENUMRECORDS

// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION) || defined(HPP_ALIB_LANG_ENUMS)
    #error "include alib/alib.hpp instead of this header"
#endif

#ifndef HPP_ALIB_LANG_ENUMS
#define HPP_ALIB_LANG_ENUMS 1

namespace aworx {

// For documentation, we are faking all operators and enum related template functions to namespace
// aworx::lib::lang
#if defined(DOX_PARSER)
namespace lib { namespace lang {
#endif


/**
 * Returns the "internal" integer type value of an enumeration value. This is useful for example,
 * to return result codes to callers of different programming languages or command line programs.
 *
 * Selected by the compiler only if template parameter \p{TEnum} represents an enum type.
 *
 * \note While documented in namespace <c>%aworx::lib::lang</c> in fact this function resides in
 *       namespace #aworx.
 *
 * \see A different, and much more far-reaching approach is implemented with class \alib{lang,Enum}.
 *
 * @param  element   The enumeration element.
 * @tparam TEnum         Enumeration type.
 * @tparam TEnableIf     Internal. Do \b not specify!<br>
 *                       (Defaults to \c std::enable_if type, to enable the compiler to select this
 *                       operator only for types that have \alib{lang,T_EnumIsBitwise} set.)
 * @return \c true if all bits of \p{testFor} are set in \p{tested}.
 */
template<typename TEnum,
         typename TEnableIf= typename  std::enable_if<std::is_enum<TEnum>::value>::type >
constexpr
typename std::underlying_type<TEnum>::type    EnumValue(TEnum  element) noexcept(true)
{
    return static_cast<typename std::underlying_type<TEnum>::type>( element );
}

// Reset documentation fake
#if defined(DOX_PARSER)
}} // namespace aworx[::lib::lang]
#endif


namespace lib { namespace lang {


/** ************************************************************************************************
 * Enumeration representing a boolean value. While the use of this enumeration type seems senseless
 * at the first sight (as the C++ has keywords \c bool, \c false and \c true), the reason for its
 * existence is to have write and parse methods in place using the concept of
 * \alib{lang,T_EnumMetaDataDecl,ALib Enum Meta Information}.
 *
 * The default name translation table is equipped with various 'overloaded' element entries like
 * "yes", "no", "on", "off", "1" or "0". Furthermore, if other languages should be supported,
 * this can even be extended at runtime.
 **************************************************************************************************/
enum class Bool
{
    False,        ///< False value
    True          ///< True value
};

/** ************************************************************************************************
 * Denotes if sth. is switched on or off.
 **************************************************************************************************/
enum class Switch
{
    Off,          ///< Switch it off, switched off, etc.
    On            ///< Switch it on, switched on, etc.
};

/** ************************************************************************************************
 * Denotes upper and lower case character treatment.
 **************************************************************************************************/
//! [DOX_ALIB_LANG_ENUMS_PARSABLE_1]
enum class Case
{
    Sensitive,    ///< Chooses an operation mode which differs between lower and upper case letters
                  ///< (usually the default).
    Ignore        ///< Chooses an operation mode which does not differ between between lower and
                  ///< upper case letters.
};
//! [DOX_ALIB_LANG_ENUMS_PARSABLE_1]

/** ************************************************************************************************
 * Denotes Alignments.
 **************************************************************************************************/
enum class Alignment
{
    Left,         ///<  Chooses left alignment.
    Right,        ///<  Chooses right alignment.
    Center        ///<  Chooses centered alignment.
};

/** ************************************************************************************************
 * Denotes sort order.
 **************************************************************************************************/
enum class SortOrder
{
    Ascending,    ///<  Chooses ascending sort oder.
    Descending,   ///<  Chooses descending sort oder.
};

/** ************************************************************************************************
 * Denotes how members of a set something should be taken into account.
 **************************************************************************************************/
enum class Inclusion
{
    Include,      ///<  Chooses inclusion.
    Exclude,      ///<  Chooses exclusion.
};

/** ************************************************************************************************
 * Denotes the reach of something.
 **************************************************************************************************/
enum class Reach
{
    Global,       ///< Denotes global reach.
    Local         ///< Denotes local reach.
};

/** ************************************************************************************************
 * Denotes if current, existing data should be cleared or if new data should be
 * appended or otherwise added.
 **************************************************************************************************/
enum class CurrentData
{
    Keep,         ///<  Chooses not no clear existing data.
    Clear,        ///<  Chooses to clear existing data.
};

/** ************************************************************************************************
 * Denotes if the source data should be moved or copied.
 **************************************************************************************************/
enum class SourceData
{
    Copy,         ///<  Chooses not to clear existing data.
    Move,         ///<  Chooses to clear existing data.
};

/** ************************************************************************************************
 * Denotes whether something should be performed in a safe or unsafe fashion.
 **************************************************************************************************/
enum class Safeness
{
    Safe,         ///<  Do it or treat it with safety
    Unsafe,       ///<  Omit checks or perform unsafe operations
};

/** ************************************************************************************************
 * Denotes whether the responsibility for something is kept or passed. This is useful, e.g. in
 * situations where objects are shared to determine if the responsibility for the deletion
 * of object is transferred over to the receiver or kept by the sender of the object.
 **************************************************************************************************/
enum class Responsibility
{
    KeepWithSender,  ///<  Keeps responsibility, e.g. when passing an object.
    Transfer,        ///<  Transfers responsibility to the receiving party.
};

/** ************************************************************************************************
 * Denotes whether a time value represents local time or UTC.
 **************************************************************************************************/
enum class Timezone
{
    Local,        ///<  Denotes local time.
    UTC,          ///<  Denotes UTC (coordinated universal time).
};

/** ************************************************************************************************
 * Denotes whether a lock should allow recursive locks (count them) or not.
 **************************************************************************************************/
enum class LockMode
{
    Recursive,    ///< Allow nested locks.
    SingleLocks   ///< Nested locks are ignored. First unlock, unlocks.
};

/** ************************************************************************************************
 * Denotes whether a string is trimmed or not
 **************************************************************************************************/
enum class Whitespaces
{
    Trim,         ///< Trim whitespaces away.
    Keep          ///< Keep whitespaces in string.
};


/** ************************************************************************************************
 * Denotes whether something should be created if it does not exist.
 **************************************************************************************************/
enum class Create
{
    Never,        ///< Do not create.
    IfNotExistent ///< Create if something does not exist.
};


/** ************************************************************************************************
 * Denotes whether a e.g a setting should be propagated.
 **************************************************************************************************/
enum class Propagation
{
    Omit,         ///< Do not propagate changes.
    ToDescendants ///< Propagate changes to descendants/children/sub-components.
};

/** ************************************************************************************************
 * Denotes a phase, e.g. of a transaction.
 **************************************************************************************************/
enum class Phase
{
    Begin         = (1 << 0),    ///< The start of a transaction.
    End           = (1 << 1),    ///< The end of a transaction.
};

/** ************************************************************************************************
 * Denotes standard container operations.
 **************************************************************************************************/
enum class ContainerOp
{
    Insert,       ///< Denotes insertions.
    Remove,       ///< Denotes removals.
    Get,          ///< Denotes to search data.
    GetCreate,    ///< Denotes to create data if not found.
    Create        ///< Denotes to create data.
};

/** ************************************************************************************************
 * Used for example with constructors that allow to suppress initialization of members.
 **************************************************************************************************/
enum class Initialization
{
    Perform,      ///< Perform initialization.
    Suppress,     ///< Suppress initialization.
};


}} // namespace [lib::lang]

/// Type alias in namespace #aworx.
using     Bool=             aworx::lib::lang::Bool;

/// Type alias in namespace #aworx.
using     Switch=           aworx::lib::lang::Switch;

/// Type alias in namespace #aworx.
using     Case=             aworx::lib::lang::Case;

/// Type alias in namespace #aworx.
using     Alignment=        aworx::lib::lang::Alignment;

/// Type alias in namespace #aworx.
using     SortOrder=        aworx::lib::lang::SortOrder;

/// Type alias in namespace #aworx.
using     Inclusion=        aworx::lib::lang::Inclusion;

/// Type alias in namespace #aworx.
using     Reach=            aworx::lib::lang::Reach;

/// Type alias in namespace #aworx.
using     CurrentData=      aworx::lib::lang::CurrentData;

/// Type alias in namespace #aworx.
using     SourceData=       aworx::lib::lang::SourceData;

/// Type alias in namespace #aworx.
using     Safeness=         aworx::lib::lang::Safeness;

/// Type alias in namespace #aworx.
using     Responsibility=   aworx::lib::lang::Responsibility;

/// Type alias in namespace #aworx.
using     Timezone=         aworx::lib::lang::Timezone;

/// Type alias in namespace #aworx.
using     LockMode=         aworx::lib::lang::LockMode;

/// Type alias in namespace #aworx.
using     Whitespaces=      aworx::lib::lang::Whitespaces;

/// Type alias in namespace #aworx.
using     Create=           aworx::lib::lang::Create;

/// Type alias in namespace #aworx.
using     Propagation=      aworx::lib::lang::Propagation;

/// Type alias in namespace #aworx.
using     Phase=            aworx::lib::lang::Phase;

/// Type alias in namespace #aworx.
using     ContainerOp=      aworx::lib::lang::ContainerOp;

/// Type alias in namespace #aworx.
using     Initialization=   aworx::lib::lang::Initialization;

}  // namespace aworx


#endif // HPP_ALIB_LANG_ENUMS

// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_LIB_COMMONENUMS
#define HPP_ALIB_LIB_COMMONENUMS 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_FILESET(COMMON_ENUMS)

#if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#   include "alib/lib/predef_compiler.hpp"
#endif

namespace aworx { namespace lib {

/** ************************************************************************************************
 * Enumeration representing a boolean value. While the use of this enumeration type seems senseless
 * at the first sight (as the C++ has keywords \c bool, \c false and \c true), the reason for its
 * existence is to have write and parse methods in place using the concept of
 * \alib{resources,T_EnumMetaDataDecl,ALib Enum Meta Information}.
 *
 * The default name translation table is equipped with various 'overloaded' element entries like
 * "yes", "no", "on", "off", "1" or "0". Furthermore, if other languages should be supported,
 * this can even be extended at run-time.
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
DOX_MARKER( [DOX_ALIB_ENUMS_PARSABLE_1] )
enum class Case
{
    Sensitive,    ///< Chooses an operation mode which differs between lower and upper case letters
                  ///< (usually the default).
    Ignore        ///< Chooses an operation mode which does not differ between between lower and
                  ///< upper case letters.
};
DOX_MARKER( [DOX_ALIB_ENUMS_PARSABLE_1] )

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
    Safe,         ///<  Do it or treat it with safety.
    Unsafe,       ///<  Omit checks or perform unsafe operations.
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
    Suppress,     ///< Suppress initialization.
                  ///< Writes "Init", reads in addition "suppress", "no", "off", "false", "0" "-".
    Perform,      ///< Perform initialization.
                  ///< Writes "NoInit", reads in addition "perform", "synchronized", "synced" "yes",
                  ///< "on", "true", "1".
};

/** ************************************************************************************************
 * Denotes if asynchronous tasks become synchronized.
 **************************************************************************************************/
enum class Timing
{
    Async,    ///< The end of a transaction.
              ///< Writes "Async", reads in addition "asynchronous", "no", "off", "false", "0" "-".
    Sync,     ///< The start of a transaction.
              ///< Writes "Sync", reads in addition "synchronous", "synchronized", "synced" "yes",
              ///< "on", "true", "1".
};


} // namespace aworx[::lib]

/// Type alias in namespace #aworx.
using     Bool=             aworx::lib::Bool;

/// Type alias in namespace #aworx.
using     Switch=           aworx::lib::Switch;

/// Type alias in namespace #aworx.
using     Case=             aworx::lib::Case;

/// Type alias in namespace #aworx.
using     Alignment=        aworx::lib::Alignment;

/// Type alias in namespace #aworx.
using     SortOrder=        aworx::lib::SortOrder;

/// Type alias in namespace #aworx.
using     Inclusion=        aworx::lib::Inclusion;

/// Type alias in namespace #aworx.
using     Reach=            aworx::lib::Reach;

/// Type alias in namespace #aworx.
using     CurrentData=      aworx::lib::CurrentData;

/// Type alias in namespace #aworx.
using     SourceData=       aworx::lib::SourceData;

/// Type alias in namespace #aworx.
using     Safeness=         aworx::lib::Safeness;

/// Type alias in namespace #aworx.
using     Responsibility=   aworx::lib::Responsibility;

/// Type alias in namespace #aworx.
using     Timezone=         aworx::lib::Timezone;

/// Type alias in namespace #aworx.
using     Whitespaces=      aworx::lib::Whitespaces;

/// Type alias in namespace #aworx.
using     Create=           aworx::lib::Create;

/// Type alias in namespace #aworx.
using     Propagation=      aworx::lib::Propagation;

/// Type alias in namespace #aworx.
using     Phase=            aworx::lib::Phase;

/// Type alias in namespace #aworx.
using     ContainerOp=      aworx::lib::ContainerOp;

/// Type alias in namespace #aworx.
using     Initialization=   aworx::lib::Initialization;

/// Type alias in namespace #aworx.
using     Timing=           aworx::lib::Timing;

}  // namespace [aworx]


#endif // HPP_ALIB_LIB_COMMONENUMS

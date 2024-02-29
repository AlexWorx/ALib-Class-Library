/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong. It does not belong to an \alibmod and is
 * included in any \alibdist.
 *
 * \note
 *   Generally, if \alib_enums is included in the \alibdist, header file
 *   \alibheader{lang/commonenums.hpp} should be included instead of this header,
 *   which, in addition to including this header, also defines
 *   \ref alib_enums_records "ALib Enum Records" for the common enums.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_COMMONENUMS_DEFS
#define HPP_ALIB_LANG_COMMONENUMS_DEFS 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

namespace alib::lang {

// CodeMarker_CommonEnums

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
 * Enumeration representing a boolean value. While the use of this enumeration type seems senseless
 * at the first sight (as the C++ has keywords \c bool, \c false and \c true), the reason for its
 * existence is to have write and parse methods in place using the concept of
 * \ref alib_enums_records "ALib Enum Records".
 *
 * The default name translation table is equipped with various 'overloaded' element entries like
 * "yes", "no", "on", "off", "1" or "0". Furthermore, if other languages should be supported,
 * this can even be extended at run-time.
 **************************************************************************************************/
enum class Bool                 : bool
{
    False,        ///< False value
    True          ///< True value
};

/** ************************************************************************************************
 * Denotes if a cache mechanism is enabled or disabled.
 **************************************************************************************************/
enum class Caching
{
    Disabled,  ///< Caching is disabled.
    Enabled,   ///< Caching is enabled.
    Auto,      ///< Auto/default mode.
};

/** ************************************************************************************************
 * Denotes upper and lower case character treatment.
 **************************************************************************************************/
DOX_MARKER( [DOX_ALIB_ENUMS_MAKE_PARSABLE_1] )
enum class Case                 : bool
{
    Sensitive,    ///< Chooses an operation mode which differs between lower and upper case letters
                  ///< (usually the default).
    Ignore        ///< Chooses an operation mode which does not differ between between lower and
                  ///< upper case letters.
};
DOX_MARKER( [DOX_ALIB_ENUMS_MAKE_PARSABLE_1] )

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
 * Denotes whether default entities should be created or not.
 **************************************************************************************************/
enum class CreateDefaults       : bool
{
    No,           ///< Do not create.
    Yes           ///< Create default values.
};

/** ************************************************************************************************
 * Denotes whether something should be created if it does not exist.
 **************************************************************************************************/
enum class CreateIfNotExists    : bool
{
    No,           ///< Do not create.
    Yes           ///< Create if something does not exist.
};

/** ************************************************************************************************
 * Denotes if current, existing data should be cleared or if new data should be
 * appended or otherwise added.
 **************************************************************************************************/
enum class CurrentData          : bool
{
    Keep,         ///<  Chooses not no clear existing data.
    Clear,        ///<  Chooses to clear existing data.
};

/** ************************************************************************************************
 * Denotes how members of a set something should be taken into account.
 **************************************************************************************************/
enum class Inclusion            : bool
{
    Include,      ///<  Chooses inclusion.
    Exclude,      ///<  Chooses exclusion.
};

/** ************************************************************************************************
 * Used for example with constructors that allow to suppress initialization of members.
 **************************************************************************************************/
enum class Initialization
{
    Suppress,     ///< Suppress initialization.
                  ///< Writes "DontInitialize", reads in addition "suppress".
    Perform,      ///< Perform initialization.
                  ///< Writes "Initialize", reads in addition "perform".
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
 * Denotes whether a e.g a setting should be propagated.
 **************************************************************************************************/
enum class Propagation          : bool
{
    Omit,         ///< Do not propagate changes.
    ToDescendants ///< Propagate changes to descendants/children/sub-components.
};

/** ************************************************************************************************
 * Denotes the reach of something.
 **************************************************************************************************/
enum class Reach                : bool
{
    Global,       ///< Denotes global reach.
    Local         ///< Denotes local reach.
};

/** ************************************************************************************************
 * Denotes whether recursion is performed/allowed or not.
 **************************************************************************************************/
enum class Recursive            : bool
{
    No,           ///< Denotes non-recursive.
    Yes           ///< Denotes recursion.
};

/** ************************************************************************************************
 * Denotes whether the responsibility for something is kept or passed. This is useful, e.g. in
 * situations where objects are shared to determine if the responsibility for the deletion
 * of object is transferred over to the receiver or kept by the sender of the object.
 **************************************************************************************************/
enum class Responsibility       : bool
{
    KeepWithSender,  ///<  Keeps responsibility, e.g. when passing an object.
    Transfer,        ///<  Transfers responsibility to the receiving party.
};

/** ************************************************************************************************
 * Denotes whether something should be performed in a safe or unsafe fashion.
 **************************************************************************************************/
enum class Safeness             : bool
{
    Safe,         ///<  Do it or treat it with safety.
    Unsafe,       ///<  Omit checks or perform unsafe operations.
};

/** ************************************************************************************************
 * Denotes if something is left or right.
 **************************************************************************************************/
enum class Side                 : bool
{
    Left,          ///< Denotes the left side of something.
    Right          ///< Denotes the right side of something.
};

/** ************************************************************************************************
 * Denotes sort order.
 **************************************************************************************************/
enum class SortOrder            : bool
{
    Ascending,    ///<  Chooses ascending sort oder.
    Descending,   ///<  Chooses descending sort oder.
};

/** ************************************************************************************************
 * Denotes if the source data should be moved or copied.
 **************************************************************************************************/
enum class SourceData           : bool
{
    Copy,         ///<  Chooses not to clear existing data.
    Move,         ///<  Chooses to clear existing data.
};

/** ************************************************************************************************
 * Denotes if sth. is switched on or off.
 **************************************************************************************************/
enum class Switch               : bool
{
    Off,          ///< Switch it off, switched off, etc.
    On            ///< Switch it on, switched on, etc.
};

/** ************************************************************************************************
 * Denotes whether a time value represents local time or UTC.
 **************************************************************************************************/
enum class Timezone             : bool
{
    Local,        ///<  Denotes local time.
    UTC,          ///<  Denotes UTC (coordinated universal time).
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

/** ************************************************************************************************
 * Denotes if a value is interpreted as an absolute or relative number.
 **************************************************************************************************/
enum class  ValueReference
{
    Absolute,  ///< Referring to an absolute value.
    Relative,  ///< Referring to a relative value.
};

/** ************************************************************************************************
 * Denotes whether a string is trimmed or not
 **************************************************************************************************/
enum class Whitespaces          : bool
{
    Trim,         ///< Trim whitespaces away.
    Keep          ///< Keep whitespaces in string.
};


} // namespace [alib::lang]




#endif // HPP_ALIB_LANG_COMMONENUMS_DEFS

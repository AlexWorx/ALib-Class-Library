/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_config of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CONFIG_PRIORITIES
#define HPP_ALIB_CONFIG_PRIORITIES 1

#if !defined (HPP_ALIB_ENUMS_ARITHMETICAL)
    #include "alib/enums/arithmetical.hpp"
#endif

ALIB_ASSERT_MODULE(CONFIGURATION)

#if !defined (HPP_ALIB_ENUMS_RECORDS)
#   include "alib/enums/records.hpp"
#endif

#if !defined (HPP_ALIB_BOXING_BOXING)
#   include "alib/boxing/boxing.hpp"
#endif

#if !defined (_GLIBCXX_NUMERIC_LIMITS) && !defined(_LIMITS_)
#   include <limits>
#endif

namespace aworx { namespace lib { namespace config {


/** ************************************************************************************************
 * \alib{enums,T_EnumIsArithmetical,Arithmetical enumeration} used to prioritize
 * of type \alib{config,ConfigurationPlugin} objects attached to instances of
 * class \alib{config,Configuration}.
 **************************************************************************************************/
enum class Priorities
{
    /** Used to denote that no variable or plug-in was found. */
    NONE                    = 0,

    /**
     * Constant providing a priority which is even lower than default. This priority is not used by
     * any standard plug-ins and currently not even anywhere in detail \alib. The rationale to
     * provide it, is for third party libraries that may use this value in cases where values are
     * estimated or detected instead of defaulted.<br>
     * A user of the module may then overwrite such auto-detection by setting a default value in
     * the configuration.<br>
     * The underlying integral value is \c 500.
     */
    AutoDetected            = 500,

    /**
     * Used to store default values. By default, a plug-in of type \alib{config,InMemoryPlugin} is
     * installed here.. The underlying integral value is \c 500. \c 10000.
     */
    DefaultValues           = 10000,


    /**
     * Used to attach the main application configuration plug-in.   For example an plug-in of type
     * \alib{config,IniFile}. The underlying integral value is \c 500. \c 20000.
     */
    Standard                = 20000,


    /**
     * Used with plug-in \alib{config,Environment}.  The underlying integral value is \c 30000.
     */
    Environment             = 30000,


    /**
     * Used with plug-in \alib{config,CLIArgs}. The underlying integral value is \c 40000.
     */
    CLI                     = 40000,

    /**
     * Used to store protected values. By default, a plug-in of type \alib{config,InMemoryPlugin} is
     * installed here. No other plug-in can overwrite configuration variables which are
     * defined in a plug-in with this priority.<br>
     * The underlying integral value is  <c>std::numeric_limits<int>::max()</c>.
     */
    ProtectedValues         = (std::numeric_limits<int>::max)(),
};

/** ************************************************************************************************
 * \ref alib_enums_records "ALib Enum Record" associated with enumeration \alib{config,Priorities}.
 **************************************************************************************************/
struct ERPriorities : enums::ERSerializable
{
    /** The precedence of an operator in respect to other binary operators. */
    int         Priority;

    /**
     * Required default constructor leaving the record undefined
     * (Requirement is documented with\alib{enums::EnumRecordPrototype}.)
     */
    ERPriorities()                                                              noexcept  = default;

    /** Implementation of \alib{enums,EnumRecordPrototype::Parse}.  */
    ALIB_API
    void Parse();
};



}} // namespace aworx::[lib::config]

/// Type alias in namespace #aworx.
using     Priorities=       aworx::lib::config::Priorities;

} // namespace [aworx]

// #################################################################################################
// Macros
// #################################################################################################
ALIB_BOXING_VTABLE_DECLARE(   aworx::lib::config::Priorities, vt_config_priorities )
ALIB_ENUMS_MAKE_ARITHMETICAL( aworx::lib::config::Priorities  )
ALIB_ENUMS_ASSIGN_RECORD(     aworx::lib::config::Priorities, aworx::lib::config::ERPriorities  )

#endif // HPP_ALIB_CONFIG_PRIORITIES

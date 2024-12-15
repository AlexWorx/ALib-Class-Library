//==================================================================================================
/// \file
/// This header file is part of module \alib_config of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_CONFIG_PRIORITIES
#define HPP_ALIB_CONFIG_PRIORITIES 1
#pragma once
#include "alib/enums/arithmetical.hpp"

ALIB_ASSERT_MODULE(CONFIGURATION)

#include "alib/enums/records.hpp"
#include "alib/boxing/boxing.hpp"
#include <limits>

namespace alib {  namespace config {


//==================================================================================================
/// \alib{enums;T_EnumIsArithmetical;Arithmetical enumeration} used to control write access to
/// configuration variables, depending on the source of assignable values.
/// @see Chapter \ref alib_config_definition_prios of the Programmer's Manual of camp
///      \alib_config_nl.
//==================================================================================================
enum class Priority : uint16_t
{
    /// This priority value denotes that a variable is undefined and has no value set.
    /// The underlying integral value is \c 0.
    NONE                    = 0,

    /// Constant providing a priority which is even lower than default. A use-case for this
    /// priority are for third party libraries that may preset variables in cases where values are
    /// estimated or detected instead of defaulted.<br>
    /// A using code of such library may then overwrite the auto-detection estimates, by setting a
    /// default value in the configuration.<br>
    /// This priority is not used internally (by any \alib camp) today.<br>
    /// The underlying integral value is \c 1,000.
    AutoDetected            = 1000,

     /// Used to store default values, either from (resourced) declarations, hard-coded values,
     /// or values provided with method \alib{config;Configuration::PresetImportString}.
     /// The underlying integral value is \c 2,000.
    DefaultValues           = 2000,

    /// This is the default priority when a variable is defined for setting a 'hard-coded' value.
    /// The underlying integral value is \c 4,000. Hard-coded values have a higher priority
    /// than default values, but are deemed to get overwritten by any other configuration source.
    Standard                = 4000,

    /// External application configuration sources use this element to define variables
    /// found. This element is also used with built-in class \alib{config;IniFileFeeder}.
    /// The underlying integral value is \c 6,000.
    ConfigFile              = 6000,

    /// Used with plug-in \alib{config;EnvironmentVariablesPlugin}.
    /// The underlying integral value is \c 8,000.
    Environment             = 8000,

    /// Used to store temporary session information. Those are higher than \b Environment but lower
    /// than \b CLI. This session priority is only a proposal. Implementations might use a
    /// different value, even for different variables, for example <b>Environment - 1 </b> or
    /// <b>CLI + 1 </b>. It depends on the use case.<br>
    /// The underlying integral value is \c 10,000.
    SessionFile             = 10000,

    /// Used with plug-in \alib{config;CLIVariablesPlugin}.
    /// The underlying integral value is \c 12,000.
    CLI                     = 12000,

    /// Used to store generate temporary session information. While usually changes made in
    /// source code has a low priority, session information is overwritten by running software, no
    /// matter from which external source an existing values came.
    /// If a software wants to disallow the change of session information imposed by a library
    /// or a different software part, still a value can be set to protected.
    /// The underlying integral value is \c 14,000.
    Session                 = 14000,

    /// Used to define variables with protected values. If all code entities apply to the
    /// \ref alib_config_definition "contract that this camp imposes" in respect to variable
    /// definitions and priorities, a value set with this priority cannot be manipulated from
    /// "outside", hence by config files, command line arguments or any custom configuration source
    /// or plug-in.
    ///
    /// The underlying integral value is <c>std::numeric_limits<int>::max()</c>.
    Protected               = (std::numeric_limits<uint16_t>::max)(),

};

//==================================================================================================
/// \ref alib_enums_records "ALib Enum Record" associated with enumeration \alib{config;Priority}.
//==================================================================================================
struct ERPriority : enums::ERSerializable
{
    /// The precedence of an operator in respect to other binary operators.
    int         Priority;

    /// Required default constructor leaving the record undefined
    /// (Requirement is documented with\alib{enums::EnumRecordPrototype}.)
    ERPriority()                                                              noexcept  = default;

    /// Implementation of \alib{enums;EnumRecordPrototype::Parse}.
    ALIB_API
    void Parse();
};

} // namespace alib::[config]

/// Type alias in namespace \b alib.
using     Priority=       alib::config::Priority;

} // namespace [alib]

// #################################################################################################
// Macros
// #################################################################################################
ALIB_BOXING_VTABLE_DECLARE(   alib::config::Priority, vt_config_priorities )
ALIB_ENUMS_MAKE_ARITHMETICAL( alib::config::Priority  )
ALIB_ENUMS_ASSIGN_RECORD(     alib::config::Priority, alib::config::ERPriority  )

#endif // HPP_ALIB_CONFIG_PRIORITIES


// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_ALOX_VARIABLES
#define HPP_ALIB_ALOX_VARIABLES 1

#if !defined (HPP_ALIB_ALOX)
#   include "alib/alox/alox.hpp"
#endif

namespace aworx { namespace lib { namespace lox {

/** ************************************************************************************************
 * Configuration variables uses by ALox
 **************************************************************************************************/
enum class Variables
{
    NO_IDE_LOGGER        = 1,  ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by \alox{Log::AddDebugLogger}.
    CONSOLE_TYPE         = 2,  ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by \alox{Lox::CreateConsoleLogger}.
    VERBOSITY            = 3,  ///< Configuration variable [ALOX_LOXNAME_LOGGERNAME_VERBOSITY](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html).
    SPTR_GLOBAL          = 4,  ///< Configuration variable [ALOX_GLOBAL_SOURCE_PATH_TRIM_RULES](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html).
    SPTR_LOX             = 5,  ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by class \alox{Lox}.
    DOMAIN_SUBSTITUTION  = 6,  ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by class \alox{Lox}.
    PREFIXES             = 7,  ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by class \alox{Lox}.
    DUMP_STATE_ON_EXIT   = 8,  ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by class \alox{Lox}.
    AUTO_SIZES           = 20, ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by class \alox{detail::textlogger,TextLogger}.
    FORMAT               = 21, ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by class \alox{detail::textlogger,TextLogger}.
    FORMAT_DATE_TIME     = 22, ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by class \alox{detail::textlogger,TextLogger}.
    FORMAT_MULTILINE     = 23, ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by class \alox{detail::textlogger,TextLogger}.
    FORMAT_TIME_DIFF     = 24, ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by class \alox{detail::textlogger,TextLogger}.
    MAX_ELAPSED_TIME     = 25, ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by class \alox{detail::textlogger,TextLogger}.
    REPLACEMENTS         = 26, ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by class \alox{detail::textlogger,TextLogger}.
    CONSOLE_LIGHT_COLORS = 27, ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by colorful specializations of class \alox{detail::textlogger,TextLogger}.

    #if defined(_WIN32)
      CODEPAGE           = 28, ///< [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) used by class \alox{loggers,WindowsConsoleLogger}.
    #endif
};


}}} // namespace [aworx::lib::lox]



ALIB_CONFIG_VARIABLES( aworx::lib::lox::Variables, aworx::lib::ALOX, "Var" )


#endif // HPP_ALIB_ALOX_VARIABLES

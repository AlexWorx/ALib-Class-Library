/** ************************************************************************************************
 * \file
 * This header file is part of the unit tests of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#include "alib/lang/callerinfo_functions.hpp"
#if ALOX_DBG_LOG
    void Log_ScopeDomains_HPPHelper();
    void Log_ScopeDomains_HPPHelper()
    {
        Log_Info("")
    }
#endif

#if ALOX_REL_LOG
    void Lox_ScopeDomains_HPPHelper(Lox& lox);
    void Lox_ScopeDomains_HPPHelper(Lox& lox)
    {
        #define LOX_LOX lox
        Lox_Info("")
        #undef LOX_LOX
    }
#endif
#include "alib/lang/callerinfo_methods.hpp"

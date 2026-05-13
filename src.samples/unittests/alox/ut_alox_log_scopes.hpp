/** ************************************************************************************************
 * \file
 * This header-file is part of the unit tests of the \aliblong.
 *
 * Copyright 2013-2026 A-Worx GmbH, Germany.
 * Published under #"mainpage_license".
 **************************************************************************************************/
#include "ALib.Lang.CIFunctions.H"
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
#include "ALib.Lang.CIMethods.H"

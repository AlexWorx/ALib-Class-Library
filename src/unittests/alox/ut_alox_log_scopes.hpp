// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#if ALOX_DBG_LOG

    void Log_ScopeDomains_HPPHelper();
    void Log_ScopeDomains_HPPHelper()
    {
        Log_Info("");
    }

#endif

#if ALOX_REL_LOG

    void Lox_ScopeDomains_HPPHelper(Lox& lox);
    void Lox_ScopeDomains_HPPHelper(Lox& lox)
    {
        #define LOX_LOX lox
        Lox_Info("");
        #undef LOX_LOX
    }
#endif

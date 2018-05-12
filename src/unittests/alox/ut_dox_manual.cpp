// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"
#include "alib/compatibility/std_string.hpp"

#include "alib/alox/loggers/memorylogger.hpp"

#include <iostream>
#include <string>
#include <vector>

#define TESTCLASSNAME       CPP_Dox_ReferenceManual
#include "../aworx_unittests.hpp"

using namespace std;
using namespace ut_aworx;

using namespace aworx;

namespace ut_alox {


UT_CLASS()

//##################################################################################################
// SAMPLE code of class ESC documentation
//##################################################################################################

void docSampleESC()
{

//! [DOX_ALOX_ESC]
Log_Info( "The result is: ", ESC::RED, 42 )
 //! [DOX_ALOX_ESC]
}


UT_METHOD( ALoxDocSamples )
{
    UT_INIT();

    UT_PRINT("*** ALox Sample tests: ESC ***")
    docSampleESC();
}


//##################################################################################################
// SAMPLE code installing a report writer
//##################################################################################################

void docSampleInstallReportWriter()
{
Log_Prune( Lox lox("RELEASE");               )
Log_Prune( Lox_Prune( Logger* myReleaseLogger= nullptr; ) )

#if ALOX_DBG_LOG
    #define LOX_LOX lox
#endif

//! [DOX_ALOX_INSTALL_REPORT_WRITER]
Log_Prune( Log::AddALibReportWriter( &LOX_LOX ); )
Log_Prune( Lox_SetVerbosity( myReleaseLogger, Verbosity::Info, lib::lox::ALoxReportWriter::LogDomain() ); )
//! [DOX_ALOX_INSTALL_REPORT_WRITER]

//! [DOX_ALOX_INSTALL_REPORT_WRITER_2]
Log_Prune( Log::RemoveALibReportWriter() );
//! [DOX_ALOX_INSTALL_REPORT_WRITER_2]



}



UT_CLASS_END
} // namespace



// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if !defined(ALIB_UT_SELECT) || defined(ALIB_UT_ALOX)


#include "alib/alox.hpp"
#include "alib/compatibility/std_characters.hpp"

#include "alib/alox/loggers/memorylogger.hpp"

#include <iostream>
#include <string>
#include <vector>

#define TESTCLASSNAME       CPP_Dox_ReferenceManual
#include "unittests/aworx_unittests.hpp"

using namespace std;
using namespace ut_aworx;

using namespace aworx;

//! [DOX_ALOX_INSTALL_REPORT_WRITER_0]
#include "alib/alox/reportwriter.hpp"
//! [DOX_ALOX_INSTALL_REPORT_WRITER_0]

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


#endif // !defined(ALIB_UT_SELECT) || defined(ALIB_UT_ALOX)

// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_UT_ALOX


#include "alib/alox.hpp"
#include "alib/compatibility/std_characters.hpp"

#include "alib/alox/loggers/memorylogger.hpp"

#include <iostream>

// Fix the method name of logging (needed for unity builds)
ALIB_WARNINGS_IGNORE_UNUSED_MACRO
#undef  ALIB_CALLER
#if defined( __GNUC__ )
#   define ALIB_CALLER    __FILE__, __LINE__, __func__
#else
#   define ALIB_CALLER    __FILE__, __LINE__, __FUNCTION__
#endif

using namespace std;
using namespace alib;


//##################################################################################################
// SAMPLE code installing a report writer
//##################################################################################################

//! [DOX_ALOX_INSTALL_REPORT_WRITER_0]
#include "alib/alox/reportwriter.hpp"
//! [DOX_ALOX_INSTALL_REPORT_WRITER_0]

void docSampleInstallReportWriter();
void docSampleInstallReportWriter()
{
Log_Prune( Lox lox("RELEASE");               )
Log_Prune( Lox_Prune( Logger* myReleaseLogger= nullptr; ) )

#if ALOX_DBG_LOG
    #define LOX_LOX lox
#endif

//! [DOX_ALOX_INSTALL_REPORT_WRITER]
Log_Prune( Log::AddALibReportWriter( &LOX_LOX ); )
Log_Prune( Lox_SetVerbosity( myReleaseLogger, Verbosity::Info, lox::ALoxReportWriter::LogDomain() ); )
//! [DOX_ALOX_INSTALL_REPORT_WRITER]

//! [DOX_ALOX_INSTALL_REPORT_WRITER_2]
Log_Prune( Log::RemoveALibReportWriter() );
//! [DOX_ALOX_INSTALL_REPORT_WRITER_2]
}


namespace ut_alox {

//##################################################################################################
// SAMPLE code of class ESC documentation
//##################################################################################################

void docSampleESC();
void docSampleESC()
{

//! [DOX_ALOX_ESC]
Log_Info( "The result is: ", ESC::RED, 42 )
 //! [DOX_ALOX_ESC]
}

}



#define TESTCLASSNAME       CPP_Dox_ReferenceManual
#include "unittests/aworx_unittests.hpp"

using namespace ut_aworx;

namespace ut_alox {

UT_CLASS

UT_METHOD( ALoxDocSamples )
{
    UT_INIT()

    UT_PRINT("*** ALox Sample tests: ESC ***")
    docSampleESC();
}


#include "unittests/aworx_unittests_end.hpp"
} // namespace

ALIB_WARNINGS_RESTORE

#endif // ALIB_UT_ALOX

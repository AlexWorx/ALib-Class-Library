// #################################################################################################
//  AWorx ALib Unit Tests
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_UT_FILES && ALIB_DEBUG

DOX_MARKER( [DOX_VARIABLES_HEADER])
// Get module ALib.Variables
#include "ALib.Variables.H"

// Get other ALib modules we need for this sample
#include "ALib.Lang.H"
#include "ALib.Bootstrap.H"
#include "ALib.Strings.H"
#include "ALib.Camp.H"
#include "ALib.Camp.Base.H"

// Set CallerInfo-macro to function-mode (instead of class-method mode)
#include "ALib.Lang.CIFunctions.H"

using namespace alib;
using namespace std;
DOX_MARKER( [DOX_VARIABLES_HEADER])
#include <iostream>
#include <sstream>
#include "ALib.Compatibility.StdStrings.H"
#include "ALib.Strings.StdIOStream.H"
#include "ALib.Strings.Monomem.H"
#include "ALib.Camp.H"
#include "ALib.Camp.Base.H"
#include "ALib.Bootstrap.H"

#define TESTCLASSNAME       UT_Dox_Config
#include "aworx_unittests.hpp"

#if defined(__clang__)
    #pragma clang diagnostic ignored "-Wmissing-prototypes"
    #pragma clang diagnostic ignored "-Wunused-variable"
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#elif defined(_MSC_VER)
    #pragma warning( disable:4100 ) // unreferenced formal parameter

#endif

//--------------------------------------------------------------------------------------------------
//--- DOCUMENTATION SAMPLES
//--------------------------------------------------------------------------------------------------

namespace std
{
    extern std::stringstream testOSConfig; // declaration (needed when clang warnings are on)
           std::stringstream testOSConfig;
}
#define cout testOSConfig
#define Shutdown  ShutdownX
#define main mainx

#include "ALib.Lang.CIFunctions.H"

namespace alib { void ShutdownX() {} }

DOX_MARKER( [DOX_VARIABLES_HELLO_WORLD])

int main( int argc, const char** argv)
{
    // Initialize ALib
    alib::ARG_C=   argc;
    alib::ARG_VN=  argv;
    alib::Bootstrap();

    // Declare a variable as follows:
    // - use the configuration class of the base camp
    // - name the variable "MY_VAR"
    // - run-time type of the variable is "S", which stands for String-type
    // The shortcut function used, also locks the configuration during variable definition.
    Variable myVar= variables::CampVariable(alib::BASECAMP, A_CHAR("MY_VAR"), A_CHAR("S") );

    // Define the variable. If this "succeeds", assign a value
    if( myVar.Define() )
        myVar= String(A_CHAR( "World" ));

    // Write the variable's string value out
    cout << "Hello " << myVar.GetString() << endl;


    // Shutdown ALib
    alib::Shutdown();

    return 0;
}
DOX_MARKER( [DOX_VARIABLES_HELLO_WORLD])

ALIB_WARNINGS_MACRO_NOT_USED_OFF
#undef main
#undef Shutdown
#undef cout
#include "aworx_callerinfo_ut.hpp"
ALIB_WARNINGS_RESTORE

using namespace std;

// ###################################   Test Class   #############################################

namespace ut_aworx {

UT_CLASS


    UT_METHOD( configMain )
    {
        UT_INIT()
        UT_PRINT("*** Documentation Sample +**")

        auto argc= alib::ARG_C;
        auto argv= alib::ARG_VN;
        mainx(0, nullptr);
        alib::ARG_C   = argc;
        alib::ARG_VN  = argv;
        ut.WriteResultFile( "DOX_VARIABLES_HELLO_WORLD.txt", testOSConfig.str(), "");

{

DOX_MARKER( [DOX_VARIABLES_DECLARATION1])
Variable myVar= variables::CampVariable(alib::BASECAMP, A_CHAR("MY_VAR"), A_CHAR("S") );
DOX_MARKER( [DOX_VARIABLES_DECLARATION1])
(void)myVar;
}
{
ALIB_LOCK_WITH(BASECAMP.GetConfig())
DOX_MARKER( [DOX_VARIABLES_DECLARATION2])
// Create an undeclared variable within the configuration of the base camp
Variable myVar= variables::CampVariable(alib::BASECAMP);

// Declare the variable
myVar.Declare( A_CHAR("MY_VAR"), A_CHAR("S") );
DOX_MARKER( [DOX_VARIABLES_DECLARATION2])
(void)myVar;
}
{
ALIB_LOCK_WITH(BASECAMP.GetConfig())
DOX_MARKER( [DOX_VARIABLES_DECLARATION3])
// create a declaration object
Declaration declBtnColor(A_CHAR("BUTTON_COLOR"), A_CHAR("S"), A_CHAR("red"),
                         A_CHAR("The color of the buttons"));

// Create an declare a variable
Variable varBtnColor= variables::CampVariable(alib::BASECAMP);
varBtnColor.Declare(&declBtnColor);
DOX_MARKER( [DOX_VARIABLES_DECLARATION3])
(void)varBtnColor;
}
{
DOX_MARKER( [DOX_VARIABLES_DECLARATION4])
Variable locale= variables::CampVariable(alib::BASECAMP, camp::Variables::LOCALE );
DOX_MARKER( [DOX_VARIABLES_DECLARATION4])
}

{
DOX_MARKER( [DOX_VARIABLES_HELLO_WORLD2])
Variable myVar= variables::CampVariable(alib::BASECAMP, A_CHAR("MY_VAR"), A_CHAR("S") );

if( myVar.Define() )
    myVar= String(A_CHAR( "World" ));
DOX_MARKER( [DOX_VARIABLES_HELLO_WORLD2])
}

{
DOX_MARKER( [DOX_VARIABLES_HELLO_WORLD3])
Variable myVar= variables::CampVariable(alib::BASECAMP, A_CHAR("MY_VAR"), A_CHAR("S") );
myVar= String(A_CHAR( "World" ));
DOX_MARKER( [DOX_VARIABLES_HELLO_WORLD3])
}

{
Variable myVar= variables::CampVariable(alib::BASECAMP, A_CHAR("MY_VAR"), A_CHAR("S") );
myVar= String(A_CHAR( "World" ));
DOX_MARKER( [DOX_VARIABLES_BUILTIN_TYPES1])
cout << "Hello " << myVar.GetString() << endl;
DOX_MARKER( [DOX_VARIABLES_BUILTIN_TYPES1])

DOX_MARKER( [DOX_VARIABLES_BUILTIN_TYPES2])
cout << "Hello " << myVar.Get<AStringPA>() << endl;
DOX_MARKER( [DOX_VARIABLES_BUILTIN_TYPES2])

ALIB_WARNINGS_IGNORE_UNUSED_VARIABLE
DOX_MARKER( [DOX_VARIABLES_BUILTIN_TYPES3])
Variable varSwitch= variables::CampVariable(alib::BASECAMP, A_CHAR("MY_SWITCH"), A_CHAR("B") );
if( varSwitch.Define() )
{
    bool oldVal= varSwitch;
    varSwitch=   false;

    //...
}
DOX_MARKER( [DOX_VARIABLES_BUILTIN_TYPES3])
}
ALIB_WARNINGS_RESTORE

{ ALIB_LOCK_WITH(BASECAMP.GetConfig())
DOX_MARKER( [DOX_VARIABLES_IMPORT])
Variable anyVar= variables::CampVariable(alib::BASECAMP);

if( anyVar.Try( A_CHAR("NOTMY_VAR") ) )
    anyVar.Import(A_CHAR( "42" ), alib::variables::Priority::Protected );
DOX_MARKER( [DOX_VARIABLES_IMPORT])
}


} // UT_METHOD

#include "aworx_unittests_end.hpp"

} //namespace


#endif //  ALIB_UT_DOCS && ALIB_DEBUG && ALIB_DEBUG

// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_UT_CAMP

#include "ALib.ALox.H"

#define TESTCLASSNAME       UT_Dox_Format

#include <iostream>
#include <sstream>
#include "ALib.Strings.StdIOStream.H"
#include "ALib.Compatibility.StdStrings.H"
#include "ALib.Strings.H"
#include "ALib.Exceptions.H"
#include "ALib.Format.H"
#include "ALib.Format.FormatterPythonStyle.H"
#include "ALib.Format.FormatterJavaStyle.H"
#include "ALib.Format.Paragraphs.H"
#include "ALib.Boxing.H"
#include "ALib.Monomem.H"

namespace std
{
    extern stringstream testOutputStream;
}
#define cout testOutputStream

using namespace std;
using namespace alib;
using namespace alib::strings;

#define Kelvin Kelvin1

DOX_MARKER( [DOX_SF_CUSTOM_APPEND_0])
struct Kelvin
{
    double value;
};
DOX_MARKER( [DOX_SF_CUSTOM_APPEND_0])

#undef Kelvin
#define Kelvin Kelvin2
struct Kelvin
{
    double value;
};

#if defined(_MSC_VER)
  __pragma(warning( push ))
  __pragma(warning( disable : 4566 ))
#endif

DOX_MARKER( [DOX_SF_CUSTOM_APPEND_2])
ALIB_STRINGS_APPENDABLE_TYPE_INLINE(Kelvin,
    NumberFormat nf;
    nf.FractionalPartWidth= 1;
    target << Dec(src.value - 273.15, &nf) << " \u2103"; // Degree Celsius symbol (small circle + letter 'C')
)
DOX_MARKER( [DOX_SF_CUSTOM_APPEND_2])

#if defined(_MSC_VER)
  __pragma(warning( pop ))
#endif

DOX_MARKER( [DOX_SF_CUSTOM_IFORMAT_1])
void FFormat_Kelvin( const Box& box, const String& formatSpecGiven, NumberFormat& nf, AString& target );
DOX_MARKER( [DOX_SF_CUSTOM_IFORMAT_1])

DOX_MARKER( [DOX_SF_CUSTOM_IFORMAT_2])
void FFormat_Kelvin( const Box& box, const String& formatSpecGiven, NumberFormat& nf, AString& target )
{
    // set default format spec (in real code, this should be using a resourced default string)
    String formatSpec=  formatSpecGiven.IsNotEmpty() ? formatSpecGiven
                                                     : A_CHAR("C2");

    // get value from boxed object
    double value= box.Unbox<Kelvin>().value;

    // get precision
    Substring precisionString= formatSpec.Substring(1);
    if( precisionString.IsNotEmpty() )
    {
        int8_t precision;
        precisionString.ConsumeDec( precision );
        nf.FractionalPartWidth= precision;
    }
    else
        nf.FractionalPartWidth= 2;

    // convert unit (or don't)
    String unit= A_CHAR("\u212A");
    if( formatSpec.CharAtStart() == 'C' )
    {
        unit= A_CHAR("\u2103");
        value= value - 273.15;
    }
    else if( formatSpec.CharAtStart() == 'F' )
    {
        unit= A_CHAR("\u2109");
        value= value *  1.8 - 459.67;
    }

    // write value
    target << Dec( value, &nf) << ' ' << unit;
}
DOX_MARKER( [DOX_SF_CUSTOM_IFORMAT_2])



#include "aworx_unittests.hpp"

namespace ut_aworx {

UT_CLASS

UT_METHOD( ManUsing )
{
    UT_INIT()
    UT_PRINT( "Module Basecamp format dox sample" )

    //--- sample 1 ---
    {
DOX_MARKER( [DOX_SF_USING_1])
AString target;

FormatterJavaStyle()  .Format( target, "The result is %s!\n",  6 * 7 );
FormatterPythonStyle().Format( target, "The result is {}!\n",  6 * 7 );

cout << target;
DOX_MARKER( [DOX_SF_USING_1])
    }
    ut.WriteResultFile( "DOX_SF_USING_1.txt", testOutputStream.str() );
    testOutputStream.str("");


    //--- sample 2 ---
    {
DOX_MARKER( [DOX_SF_USING_2])
AString                 target;

// create two formatters and concatenate them
FormatterJavaStyle formatter;
formatter.Next.InsertDerived<FormatterPythonStyle>();

// both formats string syntax versions may be used now the first formatter.
formatter.Format( target, "%s style\n",  "Java"   );
formatter.Format( target, "{} style\n",  "Python" );

cout << target;
DOX_MARKER( [DOX_SF_USING_2])
    }
    ut.WriteResultFile( "DOX_SF_USING_2.txt", testOutputStream.str() );
    testOutputStream.str("");


    //--- sample 3 ---
    {
AString                 target;
FormatterJavaStyle formatter;
formatter.Next.InsertDerived<FormatterPythonStyle>();

DOX_MARKER( [DOX_SF_USING_3])
formatter.Format( target, "---%s---{}---",  "Java", "Python" );
DOX_MARKER( [DOX_SF_USING_3])

cout << target << endl;
    }
    ut.WriteResultFile( "DOX_SF_USING_3.txt", testOutputStream.str(),"" );
    testOutputStream.str("");

    //--- sample 4,5 ---
    {
AString                 target;
FormatterJavaStyle formatter;
formatter.Next.InsertDerived<FormatterPythonStyle>();

DOX_MARKER( [DOX_SF_USING_4])
formatter.Format( target, 1,2,3 );
DOX_MARKER( [DOX_SF_USING_4])

cout << target << endl;

DOX_MARKER( [DOX_SF_USING_4b])
target <<  1 << 2 <<  3;
DOX_MARKER( [DOX_SF_USING_4b])

DOX_MARKER( [DOX_SF_USING_5])
formatter.Format( target, "--- A: {} ---",  1,  "--- B: {} ---", 2 );
DOX_MARKER( [DOX_SF_USING_5])

DOX_MARKER( [DOX_SF_USING_5b])
formatter.Format( target, "--- A: {} ------ B: {} ---",  1, 2 );
DOX_MARKER( [DOX_SF_USING_5b])

    }
    ut.WriteResultFile( "DOX_SF_USING_4.txt", testOutputStream.str(), "" );
    testOutputStream.str("");

    //--- sample 6 ---
    {
DOX_MARKER( [DOX_SF_USING_6])
AString                 target;
FormatterPythonStyle    formatter;

BoxesMA& results= formatter.GetArgContainer();

results.Add( "The results are\n" );

// calculating speed
//...
//...
results.Add( "   Speed: {} m/s\n", 42 );

// calculating mass
//...
//...
results.Add( "    Mass: {} kg\n", 75.0 );

// calculating energy
//...
//...
results.Add( "  Energy: {} Joule\n", 66150 );

try
{
    formatter.FormatArgs( target, results );
}
catch( Exception& e )
{
    e.Format( target );
}

cout << target << endl;
DOX_MARKER( [DOX_SF_USING_6])

    }
    ut.WriteResultFile( "DOX_SF_USING_6.txt", testOutputStream.str() );
    testOutputStream.str("");


    //--- Exception 1 ---
    #if ALIB_DEBUG
    {
DOX_MARKER( [DOX_SF_USING_EXC_1])
ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
try
{
    AString target;
    alib::Formatter::Default->Format(target, "Unknown syntax: {X}", "Test");
    cout << target;
}
catch(Exception& e)
{
    cout << e.Format();
}
DOX_MARKER( [DOX_SF_USING_EXC_1])

    }
    ut.WriteResultFile( "DOX_SF_USING_EXC_1.txt", testOutputStream.str() );
    testOutputStream.str("");
    #endif


#if ALIB_DEBUG

#undef Kelvin
#define Kelvin  Kelvin1

    //--- FAppend sample 1 ---
    {
DOX_MARKER( [DOX_SF_CUSTOM_APPEND_1])
Kelvin    temperature { 287.65 };
AString   target;

ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
Formatter::Default->Format(target, "The temperature is {}\n", temperature);

cout << target;
DOX_MARKER( [DOX_SF_CUSTOM_APPEND_1])

    }

    NAString repl; repl << testOutputStream.str();
    repl.SearchAndReplace( "Kelvin1", "Kelvin");
    testOutputStream.str(repl.Terminate());
    ut.WriteResultFile( "DOX_SF_CUSTOM_APPEND_1.txt", testOutputStream.str(),"" );
    testOutputStream.str("");


#undef Kelvin
#define Kelvin Kelvin2

    //--- FAppend sample 2 ---
    {

DOX_MARKER( [DOX_SF_CUSTOM_APPEND_3])
Kelvin    temperature { 287.65 };
AString   target;

target << temperature;

cout << target << endl;
DOX_MARKER( [DOX_SF_CUSTOM_APPEND_3])

    }
    ut.WriteResultFile( "DOX_SF_CUSTOM_APPEND_3.txt", testOutputStream.str(), "" );
    testOutputStream.str("");

    //--- FAppend sample 2 ---
    {
DOX_MARKER( [DOX_SF_CUSTOM_APPEND_4])
ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( Kelvin )
DOX_MARKER( [DOX_SF_CUSTOM_APPEND_4])

DOX_MARKER( [DOX_SF_CUSTOM_APPEND_5])
Kelvin    temperature { 287.65 };
AString   target;
Box       temperatureBoxed= temperature;

target << temperatureBoxed;

cout << target << endl;
DOX_MARKER( [DOX_SF_CUSTOM_APPEND_5])

    }
    ut.WriteResultFile( "DOX_SF_CUSTOM_APPEND_5.txt", testOutputStream.str(),"" );
    testOutputStream.str("");


    //--- FAppend sample 2 ---
    {
DOX_MARKER( [DOX_SF_CUSTOM_APPEND_6])
Kelvin    temperature { 287.65 };
AString   target;

ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
Formatter::Default->Format(target, "The temperature is {}", temperature);

cout << target << endl;
DOX_MARKER( [DOX_SF_CUSTOM_APPEND_6])
    }
    ut.WriteResultFile( "DOX_SF_CUSTOM_APPEND_6.txt", testOutputStream.str(),"" );
    testOutputStream.str("");


    //--- FFormat sample  ---
    {
DOX_MARKER( [DOX_SF_CUSTOM_IFORMAT_3])
// This lock is usually NOT NEEDED!
// We do this, here because this sample code is run in the unit tests, when ALib is already
// bootstrapped.
// See note in reference documentation of function BootstrapRegister()
ALIB_LOCK_RECURSIVE_WITH( alib::monomem::GLOBAL_ALLOCATOR_LOCK )

alib::boxing::BootstrapRegister<alib::format::FFormat, Kelvin >( FFormat_Kelvin );
DOX_MARKER( [DOX_SF_CUSTOM_IFORMAT_3])

DOX_MARKER( [DOX_SF_CUSTOM_IFORMAT_4])
Kelvin    temperature { 287.65 };
AString   target;

ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
Formatter::Default->Format(target, "The temperature is {:C2}\n", temperature);
Formatter::Default->Format(target, "The temperature is {:F0}\n", temperature);
Formatter::Default->Format(target, "The temperature is {:K5}\n", temperature);

cout << target;
DOX_MARKER( [DOX_SF_CUSTOM_IFORMAT_4])
    }
    ut.WriteResultFile( "DOX_SF_CUSTOM_IFORMAT_4.txt", testOutputStream.str() );
    testOutputStream.str("");


#endif // ALIB_DEBUG



} // ut method

#include "aworx_unittests_end.hpp"

} //namespace

#endif //  ALIB_UT_DOCS && ALIB_CAMP

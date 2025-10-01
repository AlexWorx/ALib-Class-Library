// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_UT_STRINGS


DOX_MARKER( [DOX_APPEND_DEFINITION])
#include "ALib.Strings.H"
#include "ALib.Strings.Calendar.H"
#include "ALib.Lang.H"
#include "ALib.Time.H"
#include "ALib.Boxing.H"

namespace alib::strings {

    template<> struct AppendableTraits<alib::time::DateTime, character, lang::HeapAllocator>
    {
        void operator()( AString& target, const alib::time::DateTime& appendable )
        {
            alib::CalendarDateTime calendarTime;
            calendarTime.Set( appendable, lang::Timezone::UTC );
            calendarTime.Format( A_CHAR("yyyy-MM-dd HH:mm"), target );
        }
    };
}
DOX_MARKER( [DOX_APPEND_DEFINITION])


//----------------------------------- main inclusion -------------------------------
#include "ALib.Strings.H"
#include "ALib.Strings.StdIOStream.H"
#include "ALib.ALox.H"

#define TESTCLASSNAME       UT_Dox_AppendTo

#include <iostream>
#include <sstream>
namespace std
{
    extern std::stringstream sample_os; // declaration (needed when clang warnings are on)
           std::stringstream sample_os;
}
#define cout sample_os

using namespace std;
using namespace alib;

void AppendToSample();
void AppendToSample()
{
DOX_MARKER( [DOX_APPEND_USE])
AString sample;
sample << "Execution Time: " <<  DateTime();
cout << sample << endl;
DOX_MARKER( [DOX_APPEND_USE])
}



void FormatSample1();
void FormatSample1()
{
DOX_MARKER( [DOX_APPEND_FORMAT1])
AString sample;
sample << 1234.56;
cout << sample << endl;
DOX_MARKER( [DOX_APPEND_FORMAT1])
}

void FormatSample2();
void FormatSample2()
{
    character oldDecPointChar= NumberFormat::Global.DecimalPointChar;
    character oldTGroupChar  = NumberFormat::Global.ThousandsGroupChar;
                               NumberFormat::Global.DecimalPointChar    =',';
                               NumberFormat::Global.ThousandsGroupChar  ='.';

DOX_MARKER( [DOX_APPEND_FORMAT2])
AString sample;
sample << Dec( 1234.56 , &NumberFormat::Global );
cout << sample << endl;
DOX_MARKER( [DOX_APPEND_FORMAT2])

    NumberFormat::Global.DecimalPointChar   = oldDecPointChar;
    NumberFormat::Global.ThousandsGroupChar = oldTGroupChar;
}


void FormatFieldSample();
void FormatFieldSample()
{
DOX_MARKER( [DOX_APPEND_FIELD])
AString centered;
centered << '<' << Field( "Hello", 15, lang::Alignment::Center ) << '>';
cout << centered << endl;
DOX_MARKER( [DOX_APPEND_FIELD])
}


#undef cout

//----------- Now to the unit tests -------------
#include "ALib.Compatibility.StdStrings.H"
#include "ALib.Strings.StdIOStream.H"
#include "aworx_unittests.hpp"

namespace ut_aworx {

UT_CLASS

UT_METHOD( DOX_APPEND )
{
    UT_INIT()

    AppendToSample();
    ut.WriteResultFile( "DOX_ASTRING_APPEND.txt", sample_os.str(),"" );

    sample_os.str(std::string());
    FormatFieldSample();
    ut.WriteResultFile( "DOX_APPEND_FIELD.txt", sample_os.str(),"" );


    sample_os.str(std::string());
    FormatSample1();
    ut.WriteResultFile( "DOX_APPEND_FORMAT1.txt", sample_os.str(),"" );

    sample_os.str(std::string());
    FormatSample2();
    ut.WriteResultFile( "DOX_APPEND_FORMAT2.txt", sample_os.str(),"" );
}


#include "aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_DOCS && ALIB_STRINGS

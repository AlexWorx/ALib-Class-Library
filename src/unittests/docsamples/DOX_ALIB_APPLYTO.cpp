// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"

#define TESTCLASSNAME       CPP_ALib_Dox_ApplyTo

//--------------------------------------------------------------------------------------------------
//--- DOCUMENTATION SAMPLES
//--------------------------------------------------------------------------------------------------
#include <iostream>
#include <sstream>
namespace std
{
    extern std::basic_stringstream<aworx::character> applyto_os; // declaration (needed when clang warnings are on)
    std::basic_stringstream<aworx::character> applyto_os;
}
#define cout applyto_os

void ApplyToSample();

//! [DOX_ALIB_APPLYTO_DEFINITION]
#include "alib/alib.hpp"
#include "alib/strings/numberformat.hpp"

// get support for  ostream operator<<() on String objects
#include "alib/compatibility/std_string.hpp"

#include <iostream>
#include <iomanip>


// Partial template function implementation of struct T_Apply for objects of type class DateTime
// (has to be declared in namespace aworx::lib::strings)
namespace aworx { namespace lib { namespace strings {

    // We have to specialize the helper struct T_Apply.
    template<> struct T_Apply<aworx::lib::time::DateTime, character> : public std::true_type
    {
        // And with it, method Apply
        static inline integer Apply( AString& target, const aworx::lib::time::DateTime& ticks )
        {
            time::CalendarDateTime calendarTime;
            calendarTime.Set( ticks, Timezone::UTC );
            calendarTime.Format( ASTR("yyyy-MM-dd HH:mm"), target );
            return 16; // The exact number is not too relevant. But has to be > 0 if something was written!
        }
    };

}}} // closing namespace
//! [DOX_ALIB_APPLYTO_DEFINITION]

using namespace std;
using namespace aworx;

void ApplyToSample();
void ApplyToSample()
{
//! [DOX_ALIB_APPLYTO_USE]
AString sample( "Today is: " );
sample <<  DateTime();
cout << sample << endl;
//! [DOX_ALIB_APPLYTO_USE]
}

void FormatSample1();
void FormatSample1()
{
//! [DOX_ALIB_APPLYTO_FORMAT1]
AString sample;
sample << 1234.56;
cout << sample << endl;
//! [DOX_ALIB_APPLYTO_FORMAT1]
}

void FormatSample2();
void FormatSample2()
{
    character oldDecPointChar= NumberFormat::Global.DecimalPointChar;
    character oldTGroupChar=   NumberFormat::Global.ThousandsGroupChar;
                           NumberFormat::Global.DecimalPointChar    =',';
                           NumberFormat::Global.ThousandsGroupChar  ='.';

//! [DOX_ALIB_APPLYTO_FORMAT2]
AString sample;
sample << Format( 1234.56 , &NumberFormat::Global );
cout << sample << endl;
//! [DOX_ALIB_APPLYTO_FORMAT2]

    NumberFormat::Global.DecimalPointChar   = oldDecPointChar;
    NumberFormat::Global.ThousandsGroupChar = oldTGroupChar;
}


void FormatFieldSample();
void FormatFieldSample()
{
//! [DOX_ALIB_APPLYTO_FIELD]
AString centered;
centered << '*' << Format::Field( "Hello", 15, Alignment::Center ) << '*';
cout << centered << endl;
//! [DOX_ALIB_APPLYTO_FIELD]
}


#undef cout

//----------- Now to the unit tests -------------
#include "../aworx_unittests.hpp"


namespace ut_aworx {



UT_CLASS()

UT_METHOD( DOX_ALIB_APPLYTO )
{
    UT_INIT();

    ApplyToSample();
    ut.WriteResultFile( "DOX_ALIB_APPLYTO.txt", applyto_os.str() );

    applyto_os.str(std::basic_string<character>());
    FormatFieldSample();
    ut.WriteResultFile( "DOX_ALIB_APPLYTO_FIELD.txt", applyto_os.str() );


    applyto_os.str(std::basic_string<character>());
    FormatSample1();
    ut.WriteResultFile( "DOX_ALIB_APPLYTO_FORMAT1.txt", applyto_os.str() );

    applyto_os.str(std::basic_string<character>());
    FormatSample2();
    ut.WriteResultFile( "DOX_ALIB_APPLYTO_FORMAT2.txt", applyto_os.str() );
}


UT_CLASS_END

} //namespace

// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if !defined(ALIB_UT_SELECT) || defined(ALIB_UT_DOCS)


//! [DOX_ALIB_APPEND_DEFINITION]
#include "alib/strings/astring.hpp"
#include "alib/time/datetime.hpp"
#include "alib/system/calendar.hpp"

namespace aworx { namespace lib { namespace strings {

    template<> struct T_Append<aworx::lib::time::DateTime>
    {
        inline void operator()( AString& target, const aworx::lib::time::DateTime& appendable )
        {
            aworx::lib::system::CalendarDateTime calendarTime;
            calendarTime.Set( appendable, Timezone::UTC );
            calendarTime.Format( A_CHAR("yyyy-MM-dd HH:mm"), target );
        }
    };

}}}
//! [DOX_ALIB_APPEND_DEFINITION]


//----------------------------------- main inclusion -------------------------------
#include "alib/alox.hpp"

#define TESTCLASSNAME       CPP_ALib_Dox_AppendTo

#include <iostream>
#include <sstream>
namespace std
{
    extern std::stringstream sample_os; // declaration (needed when clang warnings are on)
           std::stringstream sample_os;
}
#define cout sample_os

using namespace std;
using namespace aworx;

void AppendToSample();
void AppendToSample()
{
//! [DOX_ALIB_APPEND_USE]
AString sample;
sample << "Execution Time: " <<  DateTime();
cout << sample << endl;
//! [DOX_ALIB_APPEND_USE]
}



void FormatSample1();
void FormatSample1()
{
//! [DOX_ALIB_APPEND_FORMAT1]
AString sample;
sample << 1234.56;
cout << sample << endl;
//! [DOX_ALIB_APPEND_FORMAT1]
}

void FormatSample2();
void FormatSample2()
{
    character oldDecPointChar= NumberFormat::Global.DecimalPointChar;
    character oldTGroupChar=   NumberFormat::Global.ThousandsGroupChar;
                           NumberFormat::Global.DecimalPointChar    =',';
                           NumberFormat::Global.ThousandsGroupChar  ='.';

//! [DOX_ALIB_APPEND_FORMAT2]
AString sample;
sample << Format( 1234.56 , &NumberFormat::Global );
cout << sample << endl;
//! [DOX_ALIB_APPEND_FORMAT2]

    NumberFormat::Global.DecimalPointChar   = oldDecPointChar;
    NumberFormat::Global.ThousandsGroupChar = oldTGroupChar;
}


void FormatFieldSample();
void FormatFieldSample()
{
//! [DOX_ALIB_APPEND_FIELD]
AString centered;
centered << '*' << Format::Field( "Hello", 15, Alignment::Center ) << '*';
cout << centered << endl;
//! [DOX_ALIB_APPEND_FIELD]
}


#undef cout

//----------- Now to the unit tests -------------
#include "alib/compatibility/std_characters.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"
#include "unittests/aworx_unittests.hpp"

namespace ut_aworx {

UT_CLASS()

UT_METHOD( DOX_ALIB_APPEND )
{
    UT_INIT();

    AppendToSample();
    ut.WriteResultFile( "DOX_ALIB_ASTRING_APPEND.txt", sample_os.str() );

    sample_os.str(std::string());
    FormatFieldSample();
    ut.WriteResultFile( "DOX_ALIB_APPEND_FIELD.txt", sample_os.str() );


    sample_os.str(std::string());
    FormatSample1();
    ut.WriteResultFile( "DOX_ALIB_APPEND_FORMAT1.txt", sample_os.str() );

    sample_os.str(std::string());
    FormatSample2();
    ut.WriteResultFile( "DOX_ALIB_APPEND_FORMAT2.txt", sample_os.str() );
}


UT_CLASS_END

} //namespace

#endif //!defined(ALIB_UT_SELECT) || defined(ALIB_UT_DOCS)

// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"


#define TESTCLASSNAME       CPP_ALib_Dox_T_String


//--------------------------------------------------------------------------------------------------
//--- DOCUMENTATION SAMPLES
//--------------------------------------------------------------------------------------------------
#include <iostream>
#include <sstream>

namespace std
{
    extern std::basic_stringstream<aworx::character> testOutputStream1; // declaration (needed when clang warnings are on)
           std::basic_stringstream<aworx::character> testOutputStream1;
}
#define cout testOutputStream1

void Print( const aworx::NString& text );
void MyFunction();



//! [DOX_ALIB_TOAS]
#include "alib/alib.hpp"

// get support for using std::strings with ALib strings
#include "alib/compatibility/std_string.hpp"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace aworx;

//
// user defined string class sample
//
namespace myns
{
    class MyString
    {
        private:
            const char* buffer= "This is my string!";

        public:
            constexpr  const char*  GetMyBuffer() const { return buffer; }
            inline     size_t       GetMyLength() const { return strlen( buffer ); }
    };
}

//
// Template struct specialization (has to be made in namespace aworx::lib::strings)
//
namespace aworx { namespace lib { namespace strings {

    // specialize T_String struct for String creation
    template<> struct T_String <myns::MyString,char> : public std::true_type
    {
        static inline const char* Buffer( const myns::MyString& src )  { return                        src.GetMyBuffer();   }
        static inline integer     Length( const myns::MyString& src )  { return  static_cast<integer>( src.GetMyLength() ); }
    };

}}} // [namespace aworx::lib::strings]

//
// Defining a test method that takes a const reference to String
//
void Print( const aworx::NString& text )
{
    std::cout << "Print: " << text << std::endl;
}

//
// Using it
//
void MyFunction()
{
    // using method Print with core string types
    Print( "From string literal. No strlen() is performed." );

    const char* cc= "From 'char *' (strlen() is performed).";
    Print( cc );

    // using method Print with std::string types. This is possible, because we included
    // "alib/compatibility/std_string.hpp" above
    std::string  std_string( "From std::string" );

    Print( std_string );

    // using method Print with MyString
    myns::MyString myString;
    Print( myString );

    // ...this also works with pointers
    Print( &myString );

    // Method AString::Apply also checks for struct T_String, hence we can apply MyString objects
    // to objects of class AString!
    aworx::NAString as(myString);
    as                 << "<- applied to AString with constructor" << NewLine;
    as <<  myString    << "<- applied to AString as reference"     << NewLine;
    as << &myString    << "<- applied to AString as pointer";         // again, works with pointers
    Print( as );
}
//! [DOX_ALIB_TOAS]
#undef cout

//----------- Now to the unit tests -------------
#include "../aworx_unittests.hpp"

namespace ut_aworx {


UT_CLASS()

UT_METHOD( DOX_ALIB_TOAS )
{
    UT_INIT();

    MyFunction();
    ut.WriteResultFile( "DOX_ALIB_TOAS.txt", testOutputStream1.str() );

//! [DOX_ALIB_AS_DIRECTORY]
// Creating Directory object from C++ string literal
Directory bin( ASTR("/usr/bin") );

// Creating Directory object from std::string
std::basic_string<character> libPath( ASTR("/usr/lib") );
Directory lib( libPath );

// Creating Directory object from std::string given as pointer
Directory lib2( &libPath );

// Creating Directory object from ALib AString
aworx::AString srcPath( ASTR("/usr/src") );
Directory src( srcPath );

// Creating Directory object from ALib AString given as pointer
Directory src2( &srcPath );
//! [DOX_ALIB_AS_DIRECTORY]


}


UT_CLASS_END

} //namespace

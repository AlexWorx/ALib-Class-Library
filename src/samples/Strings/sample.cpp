// #################################################################################################
//  ALib C++ Library
//  String Sample
//
//  Copyright 2024 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/compatibility/std_strings_iostream.hpp"

using namespace alib;

//  ALib string classes are comparable to similar classes from other C++ libraries. Therefore
//  many features could be demonstrated here.
//
//  There is one feature which makes ALib Strings quite unique: Using template meta
//  programming (TMP),  ALib Strings can be constructed from anything that "smells like a string".
//  With only a short piece of code (provided somewhere in a header) any custom string type
//  becomes "compatible".
//
//  Consequently, a method which expects an ALib String as a parameter, accepts just any string type!
//  This is why we call ALib strings to be "least intrusive".

void AcceptAnyString( const alib::NString& s );
void AcceptAnyString( const alib::NString& s )
{
    std::cout << "  The original string type was: " << s  << std::endl;
}


int main()
{
    // test strings
    std::string      stdString ( "std::string"      );
    std::string      stdStringP( "std::string *"    );

    alib::NAString   aString   ( "alib::AString"   );
    alib::NAString   aStringP  ( "alib::AString *" );



    AcceptAnyString(                          "char[7]"      );
    AcceptAnyString( static_cast<const char*>("const char*")  );

    AcceptAnyString(   stdString  );
    AcceptAnyString(     aString  );

    // QTString compatibility becomes available in QT projects with
    //   #include "alib/compatibility/qt_strings.hpp"
    // and you can quickly make any other custom C++ string type compatible as well.
}

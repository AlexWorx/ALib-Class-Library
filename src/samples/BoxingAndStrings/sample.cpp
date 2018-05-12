// #################################################################################################
//  ALib - A-Worx Utility Library
//  Boxing And Strings Sample
//
//  Copyright 2018 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#include "alib/compatibility/std_string.hpp"
#include "alib/compatibility/std_iostream.hpp"

#include "alib/strings/format/formatterpythonstyle.hpp"

using namespace aworx;

// Note
// There is a lot to say and demonstrate about both "ALib Boxing" and "ALib Strings".
// This is a quick demo of the combined distribution "ALib BoxingAndStrings". The most obvious
// benefits of combining both libraries are
// - to be able to apply (append) any boxed value to an AString
// - to have type safe formatter classes which allow to format "anything" into AString representation.
//
// Such formatter classes reside in namespace "aworx::lib::strings", but are not included in the
// plain ALib String module distribution (due to the lack of ALib Boxing).
//
// Therefore, the formatting options and classes is what we exclusively want to demonstrate here.
// For other samples, documentation and inspiration consult the complete ALib distribution
// at https://github.com/AlexWorx/ALox-Logging-Library.

void ApplyToAString( const Box& box )
{
    AString aString("The object/value passed is: " );

    aString << box; // "Apply" box object. Usually this appends a string representation of it.

    std::cout << aString << std::endl;
}


int main()
{
    // it is important to initialize ALib once on bootstrap
    lib::ALIB.Init();

    // simple "applying" any type of object to AString
    ApplyToAString( "Hello ALib Boxing & Stings" );
    ApplyToAString( 12345 );
    ApplyToAString( 3.1415 );

    std::cout << std::endl;


    // quick, simple ALib formatter test
    AString                 target;
    FormatterPythonStyle    formatter;
    const char*             formatString= "Host name: {!Tab20}\\n"
                                          "Result: {!Tab20!Q} \\n"
                                          "Calculation time: {!Tab20:,} years";

    formatter.Format( target, formatString,
                     "Deep Thought",  6 * 7, 7500000 );

    std::cout << target <<  std::endl;

}

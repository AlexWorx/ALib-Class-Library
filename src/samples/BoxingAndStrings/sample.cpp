// #################################################################################################
//  ALib C++ Library
//  Boxing And Strings Sample
//
//  Copyright 2019 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/compatibility/std_characters.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"

#include "alib/stringformat/formatterpythonstyle.hpp"
#include "alib/lib/alibmodules.hpp"

using namespace aworx;

// Note
// There is a lot to say and demonstrate about both "ALib Boxing" and "ALib Strings".
// This is a quick demo of the combined distribution "ALib BoxingAndStrings". The most obvious
// benefits of combining both libraries are
// - to be able to append any boxed value to an AString.
// - to have type-safe formatter classes which allow to format "anything" into AString representation.
//
// Such formatter classes reside in namespace "aworx::lib::strings", but are not included in the
// plain ALib String module distribution (due to the lack of ALib Boxing).
//
// Therefore, the formatting options and classes is what we exclusively want to demonstrate here.
// For other samples, documentation and inspiration consult the complete ALib distribution
// at https://github.com/AlexWorx/ALox-Logging-Library.

void AppendToAString( const Box& box );
void AppendToAString( const Box& box )
{
    AString aString("The object/value passed is: " );

    aString << box; // "Append" box object. This calls box-function FAppend.

    std::cout << aString << std::endl;
}


int main()
{
    // it is important to initialize ALib once on bootstrap
    aworx::ALIB.Init();

    // Simple appending of different boxed types to an AString.
    // Note: This is sampled here, because with module STRINGFORMAT, modules Strings and Boxing are
    //       included, which allows to append boxes to AString objects.)
    AppendToAString( "Hello ALib Stringformat" );
    AppendToAString( 12345 );
    AppendToAString( 3.1415 );

    std::cout << std::endl;


    // A quick, simple ALib formatter test
    AString                 target;
    FormatterPythonStyle    formatter;
    const char*             formatString= "Host name: {!Tab20}\\n"
                                          "Result: {!Tab20!Q} \\n"
                                          "Calculation time: {!Tab20:,} years";

    formatter.Format( target, formatString,
                     "Deep Thought",  6 * 7, 7500000 );

    std::cout << target <<  std::endl;

}

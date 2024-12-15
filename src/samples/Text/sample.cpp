// #################################################################################################
//  ALib C++ Library
//  Boxing And Strings Sample
//
//  Copyright 2024 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/compatibility/std_strings.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"
#include "alib/lang/format/formatterpythonstyle.hpp"


using namespace alib;

// Note
// There is a lot to say and demonstrate about both "ALib Boxing" and "ALib Strings".
// This is a quick demo of the combined distribution "ALib BoxingAndStrings". The most obvious
// benefits of combining both libraries are
// - to be able to append any boxed value to an AString.
// - to have type-safe formatter classes which allow to format "anything" into AString representation.
//
// Such formatter classes reside in namespace "alib::strings", but are not included in the
// plain ALib String module distribution (due to the lack of ALib Boxing).
//
// Therefore, the formatting options and classes is what we exclusively want to demonstrate here.
// For other samples, documentation and inspiration consult the complete ALib distribution
// at https://alib.dev.

void AppendToAString( const Box& box );
void AppendToAString( const Box& box )
{
    AString aString("The object/value passed is: " );

    aString << box; // "Append" box object. This calls box-function FAppend.

    std::cout << aString << std::endl;
}

// we "misuse" this sample application to document some feature of module Monomem
DOX_MARKER([DOX_MONOMEM_GLOBALALLOCATOR_CHANGE_DEFAULT])
#include "alib/monomem/globalallocator.hpp"

int main( int argc, const char** argv )
{
    // before bootstrapping, we initialize the global allocator "manually" to determine
    // the size of its initial buffer ourselves:
    new (&alib::monomem::GLOBAL_ALLOCATOR) MonoAllocator(ALIB_DBG("GlobalAllocator", ) 1234, 150);

    // it is important to initialize ALib once on bootstrap
    alib::ARG_C = argc;
    alib::ARG_VN= argv;
    alib::Bootstrap();

    //...
    //...
DOX_MARKER([DOX_MONOMEM_GLOBALALLOCATOR_CHANGE_DEFAULT])

    // Simple appending of different boxed types to an AString.
    // Note: This is sampled here, because with module CAMP, modules Strings and Boxing are
    //       included, which allows appending boxes to AString objects.)
    AppendToAString( "Hello ALib" );
    AppendToAString( 12345 );
    AppendToAString( 3.1415 );

    std::cout << std::endl;


    // A quick, simple ALib formatter test
    AString                 target;
    FormatterPythonStyle    formatter;
    const char*             formatString= "Host name: {!Tab20}\n"
                                          "Result: {!Tab20!Q}\n"
                                          "Calculation time: {!Tab20:,} years";

    formatter.Format( target, formatString,
                     "Deep Thought",  6 * 7, 7500000 );

    std::cout << target <<  std::endl;

    // shutdown the library and exit
    alib::Shutdown();
}

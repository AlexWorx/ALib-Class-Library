// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if !defined(ALIB_UT_SELECT) || defined(ALIB_UT_DOCS)

#include "alib/alox.hpp"


#define TESTCLASSNAME       CPP_ALib_Dox_Strings

#include <iostream>
#include <sstream>


// get support for  ostream operator<<() on String objects
#include "alib/compatibility/std_characters.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"
#include "alib/strings/substring.hpp"
#include "alib/stringformat/propertyformatters.hpp"
#include "unittests/aworx_unittests.hpp"
#include "alib/alox/logtools.hpp"

#if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#    include "alib/resources/resources.hpp"
#endif

namespace std
{
    extern stringstream testOutputStream;
}
#define cout testOutputStream

using namespace std;
using namespace aworx;
using namespace aworx::lib::strings;


//##################################################################################################
// Character literals
//##################################################################################################
void characterLiterals1()
{

{
//! [DOX_ALIB_CHARACTERS_LITERALS_1]
    char     c= 'N';
    wchar_t wc= 'W';
//! [DOX_ALIB_CHARACTERS_LITERALS_1]
    (void) c;
    (void) wc;
}

{
//! [DOX_ALIB_CHARACTERS_LITERALS_2]
    const char*     c=  "N";
    const wchar_t* wc= L"W"; // omitting prefix 'L' causes a compilation error.
//! [DOX_ALIB_CHARACTERS_LITERALS_2]
    (void) c;
    (void) wc;
}

{
//! [DOX_ALIB_CHARACTERS_LITERALS_3]
    const char*      c=  "1-byte";
    const wchar_t*  wc= L"2- or 4-bytes"; // omitting prefix 'L' causes a compilation error.
    const char16_t* w2= u"2-bytes";       // omitting prefix 'u' causes a compilation error.
    const char32_t* w4= U"4-bytes";       // omitting prefix 'U' causes a compilation error.
//! [DOX_ALIB_CHARACTERS_LITERALS_3]
    (void) c;
    (void) wc;
    (void) w2;
    (void) w4;
}

{
//! [DOX_ALIB_CHARACTERS_LITERALS_4]
    const character*   myString=  A_CHAR( "A string of default character width." );
//! [DOX_ALIB_CHARACTERS_LITERALS_4]
    (void) myString          ;
}

{
//! [DOX_ALIB_CHARACTERS_LITERALS_5]
    String           myString          = A_CHAR ( "A string of default character width."         );
    ComplementString myComplementString= A_CCHAR( "A string of complement character width."      );
    StrangeString    myStrangeString   = A_SCHAR( "A string of the 'other' wide character type." );

    NString          myNarrowString    = A_NCHAR( "A narrow string (1-byte character width)."    );
    NString          myNarrowString2   = "Macro A_NCHAR can always be omitted";

    WString          myWideString      = A_WCHAR( "A string of default wide character width (2 or 4 bytes)." );

    XString          myXString         = A_XCHAR( "A string of 'the other' wide type (2 or 4 bytes)."        );
    XString          myXString2        = A_SCHAR( "Also works with strangeChar-macro, but this is inconsistent use." );
//! [DOX_ALIB_CHARACTERS_LITERALS_5]
    (void) myString          ;
    (void) myComplementString;
    (void) myStrangeString   ;
    (void) myNarrowString    ;
    (void) myNarrowString2   ;
    (void) myWideString      ;
    (void) myXString         ;
    (void) myXString2        ;
}

}



//##################################################################################################
// SAMPLE code of Non-Checking method variants
//##################################################################################################

void parse( Substring line );
//! [DOX_ALIB_STRINGS_NONCHECKING_1]
void parse( NSubstring line )
{
    constexpr NString startToken= "<start>";
    integer idx= line.IndexOf( startToken );
    if( idx >= 0 )
    {
        line.ConsumeChars( idx + startToken.Length() );
        //...
//! [DOX_ALIB_STRINGS_NONCHECKING_1]
    }
}

namespace avoidDouble {
void parse( Substring line );
//! [DOX_ALIB_STRINGS_NONCHECKING_11]
void parse( NSubstring line )
{
    constexpr NString startToken= "<start>";
    integer idx= line.IndexOf<true>( startToken );            // <-- Explicit invocation performing checks
    if( idx >= 0 )
    {
        line.ConsumeChars<true>( idx + startToken.Length() ); // <-- Explicit invocation performing checks
        //...
//! [DOX_ALIB_STRINGS_NONCHECKING_11]
    }
}}


void sample22( NSubstring line );
void sample22( NSubstring line )
{
    constexpr NString startToken= "<start>";
    integer idx= line.IndexOf<true>( startToken, 0 );
//! [DOX_ALIB_STRINGS_NONCHECKING_2]
    if( idx >= 0 )
    {
        line.ConsumeChars<false>( idx + startToken.Length() ); // Non-checking invocation
        //...
//! [DOX_ALIB_STRINGS_NONCHECKING_2]
    }
}

void TakeStrings( const String& s1, const AString& s2, const String64 s3 );

//! [DOX_ALIB_STRINGS_AS_PARAMETERS_1]
void TakeStrings( const String& s1, const AString& s2, const String64 s3 )
//! [DOX_ALIB_STRINGS_AS_PARAMETERS_1]
{
    (void) s1;
    (void) s2;
    (void) s3;
}

void InvokeTakeStrings();
void InvokeTakeStrings()
{
/*
//! [DOX_ALIB_STRINGS_AS_PARAMETERS_2]
TakeStrings( A_CHAR("Str1"), A_CHAR("Str2"), A_CHAR("Str3") );      // Error, AString not implicitly constructable
//! [DOX_ALIB_STRINGS_AS_PARAMETERS_2]
*/
}

void InvokeTakeStrings2();
void InvokeTakeStrings2()
{
//! [DOX_ALIB_STRINGS_AS_PARAMETERS_3]
TakeStrings( A_CHAR("Str1"), AString(A_CHAR("Str2")), A_CHAR("Str3") ); // OK, AString explicit, String and LocalString implicit
//! [DOX_ALIB_STRINGS_AS_PARAMETERS_3]

}

//! [DOX_ALIB_STRINGS_ASPREALLOC_MEMBER]
class MyClass
{
    LocalString<20> name=  A_CHAR("(none)");
};
//! [DOX_ALIB_STRINGS_ASPREALLOC_MEMBER]


// ####################################### PropertyFormatter #######################################
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_1]
enum class Hobbies
{
    Hacking,
    FineArts,
};

struct  Person
{
    String        Name;
    int           Age;
    Hobbies       Hobby;
};
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_1]

//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_2]
namespace
{
    Box getName  ( const Box& p, AString&  ) { return  p.Unbox<Person*>()->Name;  }
    Box getAge   ( const Box& p, AString&  ) { return  p.Unbox<Person*>()->Age;   }
    Box getHobby ( const Box& p, AString&  ) { return  p.Unbox<Person*>()->Hobby == Hobbies::Hacking
                                                       ? A_CHAR("hacking") : A_CHAR("fine arts")  ;      }
}
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_2]

//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_3]
PropertyFormatter::TCallbackTable  PersonCallbacks=
{
      { A_CHAR("name")  , 1, getName  },
      { A_CHAR("age")   , 1, getAge   },
      { A_CHAR("hobby") , 1, getHobby },
};
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_3]


//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_1]
enum class PersonFormats
{
    Short,
    Default,
    All
};
ALIB_CONFIG_VARIABLES( PersonFormats, aworx::ALIB, "PersonFormats" )
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_1]



namespace ut_aworx {

UT_CLASS()

UT_METHOD( SimpleCodeSamples )
{
//! [DOX_ALIB_STRINGS_NULLED_AND_EMPTY]
String nulled;                // constructs a nulled string
String empty( A_CHAR("") );   // constructs an empty but not nulled string

assert(  nulled.IsNull()    );
assert(  nulled.IsEmpty()   );
assert(  empty.IsNotNull()  );
assert(  empty.IsEmpty()    );

assert(  nulled != empty    );
//! [DOX_ALIB_STRINGS_NULLED_AND_EMPTY]

(void) empty;
(void) nulled;

//! [DOX_ALIB_ASTRING_NULLED]
// Default constructor does not allocate a buffer, yet. The instance is "nulled".
AString aString;
assert(  aString.IsNull()                 );
assert(  aString == NullString()          );
assert(  aString.IsEmpty()                );
assert(  aString != EmptyString()         );

// Append an empty string. This allocates a buffer. Now the AString is not nulled any more.
aString << "";
assert(  aString.IsNotNull()              );
assert(  aString != NullString()          );
assert(  aString.IsEmpty()                );
assert(  aString == EmptyString()         );

// Append something.  Now the AString is not nulled and not empty.
aString << "ABC";
assert(  aString.IsNotNull()              );
assert(  aString != NullString()          );
assert(  aString.IsNotEmpty()             );
assert(  aString != EmptyString()         );

// Clear the contents
aString.Reset();
assert(  aString.IsNotNull()              );
assert(  aString != NullString()          );
assert(  aString.IsEmpty()                );
assert(  aString == EmptyString()         );

// Set nulled: disposes the allocated buffer. A seldom use case!
aString.SetNull();
assert(  aString.IsNull()                 );
assert(  aString == NullString()          );
assert(  aString.IsEmpty()                );
assert(  aString != EmptyString()         );
//! [DOX_ALIB_ASTRING_NULLED]

//! [DOX_ALIB_ASTRING_MODIFY_CONST_BUFFER]
const AString myAString( "Hello" );


// this would not compile because AString::SetCharAt() is not declared const....
//myAString.SetCharAt( 1, 'e' );

// ...but method VBuffer() is declared const.
myAString.VBuffer()[1]= 'e';
//! [DOX_ALIB_ASTRING_MODIFY_CONST_BUFFER]

}

UT_METHOD( Construction )
{
    UT_INIT();

//! [DOX_ALIB_CONSTRUCTION]
// Creating Directory object from C++ string literal
Directory dir1( A_CHAR("/usr/bin") );

// Creating Directory object from a std::string
std::basic_string<character> stdString( A_CHAR("/usr/bin") );
Directory dir2( stdString );

// Creating Directory object from an AString
aworx::AString aString( A_CHAR("/usr/bin") );
Directory dir3( aString );

// Creating Directory object from a Substring
aworx::Substring subString= aString.Substring(0, 4);
Directory dir4( subString );
//! [DOX_ALIB_CONSTRUCTION]

}



UT_METHOD( PropertyFormatter )
{
    UT_INIT();
    UT_PRINT( "ALib PropertyFormatter tests and documentation sample" );
    {
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_4]

// Our data objects
Person p1= { A_CHAR("Sue") , 28, Hobbies::Hacking };
Person p2= { A_CHAR("John"), 35, Hobbies::Hacking };

// The format string. Make this changeable at run-time, e.g. load from INI-file!
String format= A_CHAR("{@name} is aged {@age} and his/her hobby is {@hobby}");

// create a formatter
aworx::PropertyFormatter  propertyFormatter( format, PersonCallbacks  );

// format the two data objects
AString target;
propertyFormatter.Format( target, p1 );
target << NewLine();
propertyFormatter.Format( target, p2 );
target << NewLine();

// that's it!
std::cout << target;

//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_4]
    }
    ut.WriteResultFile( "DOX_ALIB_STRINGS_PROPERTY_FORMATTER.txt", testOutputStream.str() );
    testOutputStream.str("");

    // do some more unit testing
    {
        bool caught= false;
        try
        {
            aworx::PropertyFormatter  propertyFormatter( A_CHAR("Test {@Unknown}"), PersonCallbacks  );
        }
        catch( Exception& e )
        {
            UT_PRINT( "Exception caught as expected: " );
            LogTools::Exception( ut.lox, e, Verbosity::Info, ut.Domain, A_CHAR("  ") );
            if( e.Type() != lib::stringformat::Exceptions::UnknownPropertyInFormatString  )
            {
              UT_PRINT( "But wrong type: caught: {}, expected: {}",
                        e.Type(),
                        lib::stringformat::Exceptions::UnknownPropertyInFormatString );
              UT_TRUE( false );
            }

            caught= true;
        }
        if( !caught )
        {
            UT_PRINT( "No Exception caught. Expected: ", lib::stringformat::Exceptions::UnknownPropertyInFormatString );
            UT_TRUE( caught );
        }

    }

}


UT_METHOD( PropertyFormatters )
{
    UT_INIT();
    UT_PRINT( "ALib PropertyFormatters tests and documentation sample" );

//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_2]
aworx::ALIB.Resources->AddBulk( aworx::ALIB.ResourceCategory,

    // Enum meta data for enum class "PersonFormats"
    "PersonFormats"      ,
        A_CHAR("0|FORMATS|"   "SHORT"     "|PFVal0"  "||||"  "PFComnt|"
               "1|FORMATS|"   "DEFAULT"   "|PFVal1"  "||||"  "PFComnt|"
               "2|FORMATS|"   "ALL"       "|PFVal2"  "||||"  "PFComnt"  ),

        // Built-in default values for the variables
        "PFVal0"         ,  A_CHAR("{@name}")                           ,
        "PFVal1"         ,  A_CHAR("{@name} ({@age})")                  ,
        "PFVal2"         ,  A_CHAR("{@name} aged {@age} loves {@hobby}"),

        // Variable comments. These are written for example to an INI-file if the application
        // fetches default values at the end of the process. In this sample, all variables share
        // the same comment.
        "PFComnt"        , A_CHAR("A property format string for printing \"Persons\".\n"
                                  "You can use @name, @age and @hobby as placeholders for person attributes."),

        nullptr
);
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_2]

//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_31]
aworx::PropertyFormatters<Person, PersonFormats> PersonFormatterMap( PersonCallbacks,
                                                                     *aworx::ALIB.Config );
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_31]

//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_32]
using  FMTPerson= aworx::lib::stringformat::PropertyFormatterMapAppendable<Person, PersonFormats>;
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_32]


//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_4]
// Our data objects
Person sue=  { A_CHAR("Sue") , 28, Hobbies::Hacking };
Person john= { A_CHAR("John"), 35, Hobbies::Hacking };

// format the two data objects
AString target;
target << FMTPerson( PersonFormatterMap, PersonFormats::Short, sue  ) << NewLine();
target << FMTPerson( PersonFormatterMap, PersonFormats::All,   john ) << NewLine();

std::cout << target;
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_4]
    ut.WriteResultFile( "DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP.txt", testOutputStream.str() );
    testOutputStream.str("");
    target.Reset();
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_5]

#define FMT_PERSON(o,p)        FMTPerson( PersonFormatterMap, o                     , p )
#define FMT_PERSON_DEFAULT(p)  FMTPerson( PersonFormatterMap, PersonFormats::Default, p )
#define FMT_PERSON_SHORT(p)    FMTPerson( PersonFormatterMap, PersonFormats::Short  , p )
#define FMT_PERSON_ALL(p)      FMTPerson( PersonFormatterMap, PersonFormats::All    , p )

target << FMT_PERSON( PersonFormats::Short, sue )     << NewLine();
target << FMT_PERSON_SHORT( sue )                     << NewLine();
target << FMT_PERSON_DEFAULT( sue )                   << NewLine();
target << FMT_PERSON_ALL( sue )                       << NewLine();

std::cout << target;
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_5]

  ut.WriteResultFile( "DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_2.txt", testOutputStream.str() );
  testOutputStream.str("");
    target.Reset();

//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_6]
ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FMTPerson* )
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_6]


//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_7]
GetDefaultFormatter()->Format( target, "The person is: {}", FMT_PERSON_DEFAULT( john ) );

std::cout << target << endl;
//! [DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_7]
  ut.WriteResultFile( "DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_3.txt", testOutputStream.str() );
  testOutputStream.str("");
  target.Reset();
}


UT_CLASS_END

}; //namespace

#endif //!defined(ALIB_UT_SELECT) || defined(ALIB_UT_DOCS)

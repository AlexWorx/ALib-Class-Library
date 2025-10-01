// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"

#if ALIB_UT_DOCS && ALIB_UT_STRINGS
#include <iostream>
#include <sstream>
#include <assert.h>

#include "ALib.Lang.CIMethods.H"

#include "ALib.Camp.Base.H"
#include "ALib.ALox.H"
#include "ALib.EnumOps.H"
#include "ALib.Monomem.H"
#include "ALib.Strings.StdIOStream.H"
#include "ALib.Compatibility.StdStrings.H"
#include "ALib.Strings.H"
#include "ALib.EnumRecords.H"
#include "ALib.Exceptions.H"
#include "ALib.Format.PropertyFormatter.H"
#include "ALib.Format.PropertyFormatters.H"
#include "ALib.Format.H"
#include "ALib.EnumRecords.Bootstrap.H"
#include "ALib.Variables.H"

#include "ALib.Resources.H"

namespace std
{
    extern stringstream testOutputStream;
}
#define cout testOutputStream

using namespace std;
using namespace alib;
using namespace alib::strings;

#define TESTCLASSNAME       UT_Dox_Strings
#include "aworx_unittests.hpp"


//##################################################################################################
// Character literals
//##################################################################################################
void characterLiterals1()
{
{
DOX_MARKER( [DOX_CHARACTERS_LITERALS_1])
    char     c= 'N';
    wchar_t wc= 'W';
DOX_MARKER( [DOX_CHARACTERS_LITERALS_1])
    (void) c;
    (void) wc;
}

{
DOX_MARKER( [DOX_CHARACTERS_LITERALS_2])
    const char*     c=  "N";
    const wchar_t* wc= L"W"; // omitting prefix 'L' causes a compilation error.
DOX_MARKER( [DOX_CHARACTERS_LITERALS_2])
    (void) c;
    (void) wc;
}

{
DOX_MARKER( [DOX_CHARACTERS_LITERALS_3])
    const char*      c=   "1-byte";
    const wchar_t*  wc=  L"2- or 4-bytes"; // omitting prefix 'L' causes a compilation error.
    const char8_t*  u8= u8"1-byte" ;       // omitting prefix 'u8' causes a compilation error.
    const char16_t* u16= u"2-bytes";       // omitting prefix 'u' causes a compilation error.
    const char32_t* u32= U"4-bytes";       // omitting prefix 'U' causes a compilation error.
DOX_MARKER( [DOX_CHARACTERS_LITERALS_3])
    (void) c;
    (void) wc;
    (void) u8;
    (void) u16;
    (void) u32;
}

{
DOX_MARKER( [DOX_CHARACTERS_LITERALS_4])
    const character*   myString=  A_CHAR( "A string of default character width." );
DOX_MARKER( [DOX_CHARACTERS_LITERALS_4])
    (void) myString          ;
}

{
DOX_MARKER( [DOX_CHARACTERS_LITERALS_5])
    String           myString          = A_CHAR ( "A string of default character width."         );
    ComplementString myComplementString= A_CCHAR( "A string of complement character width."      );
    StrangeString    myStrangeString   = A_SCHAR( "A string of the 'other' wide character type." );

    NString          myNarrowString    = A_NCHAR( "A narrow string (1-byte character width)."    );
    NString          myNarrowString2   = "Macro A_NCHAR can always be omitted";

    WString          myWideString      = A_WCHAR( "A string of default wide character width (2 or 4 bytes)." );

    XString          myXString         = A_XCHAR( "A string of 'the other' wide type (2 or 4 bytes)."        );
    XString          myXString2        = A_SCHAR( "Also works with strangeChar-macro, but this is inconsistent use." );
DOX_MARKER( [DOX_CHARACTERS_LITERALS_5])
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
DOX_MARKER( [DOX_STRINGS_NONCHECKING_1])
void parse( NSubstring line )
{
    constexpr NString startToken= "<start>";
    integer idx= line.IndexOf( startToken );
    if( idx >= 0 )
    {
        line.ConsumeChars( idx + startToken.Length() );
        //...
DOX_MARKER( [DOX_STRINGS_NONCHECKING_1])
    }
}

namespace avoidDouble {
void parse( Substring line );
DOX_MARKER( [DOX_STRINGS_NONCHECKING_11])
void parse( NSubstring line )
{
    constexpr NString startToken= "<start>";
    integer idx= line.IndexOf<CHK>( startToken );            // <-- Explicit invocation performing checks
    if( idx >= 0 )
    {
        line.ConsumeChars<CHK>( idx + startToken.Length() ); // <-- Explicit invocation performing checks
        //...
DOX_MARKER( [DOX_STRINGS_NONCHECKING_11])
    }
}}


void sample22( NSubstring line );
void sample22( NSubstring line )
{
    constexpr NString startToken= "<start>";
    integer idx= line.IndexOf<CHK>( startToken, 0 );
DOX_MARKER( [DOX_STRINGS_NONCHECKING_2])
    if( idx >= 0 )
    {
        line.ConsumeChars<NC>( idx + startToken.Length() ); // Non-checking invocation
        //...
DOX_MARKER( [DOX_STRINGS_NONCHECKING_2])
    }
}

void TakeStrings( const String& s1, const AString& s2, const String64 s3 );

DOX_MARKER( [DOX_STRINGS_AS_PARAMETERS_1])
void TakeStrings( const String& s1, const AString& s2, const String64 s3 )
DOX_MARKER( [DOX_STRINGS_AS_PARAMETERS_1])
{
    (void) s1;
    (void) s2;
    (void) s3;
}

void InvokeTakeStrings();
void InvokeTakeStrings()
{
/*
DOX_MARKER( [DOX_STRINGS_AS_PARAMETERS_2])
TakeStrings( A_CHAR("Str1"), A_CHAR("Str2"), A_CHAR("Str3") );      // Error, AString not implicitly constructable
DOX_MARKER( [DOX_STRINGS_AS_PARAMETERS_2])
*/
}

void InvokeTakeStrings2();
void InvokeTakeStrings2()
{
DOX_MARKER( [DOX_STRINGS_AS_PARAMETERS_3])
TakeStrings( A_CHAR("Str1"), AString(A_CHAR("Str2")), A_CHAR("Str3") ); // OK, AString explicit, String and LocalString implicit
DOX_MARKER( [DOX_STRINGS_AS_PARAMETERS_3])

}

DOX_MARKER( [DOX_STRINGS_ASPREALLOC_MEMBER])
class MyClass
{
    LocalString<20> name=  A_CHAR("(none)");
};
DOX_MARKER( [DOX_STRINGS_ASPREALLOC_MEMBER])



// ####################################### PropertyFormatter #######################################
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_1])
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
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_1])

DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_2])
namespace
{
    Box getName  ( const Box& p, AString&  ) { return  p.Unbox<Person*>()->Name;  }
    Box getAge   ( const Box& p, AString&  ) { return  p.Unbox<Person*>()->Age;   }
    Box getHobby ( const Box& p, AString&  ) { return  p.Unbox<Person*>()->Hobby == Hobbies::Hacking
                                                       ? A_CHAR("hacking") : A_CHAR("fine arts")  ;      }
}
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_2])

static
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_3])
PropertyFormatter::TCallbackTable  PersonCallbacks=
{
      { A_CHAR("name")  , 1, getName  },
      { A_CHAR("age")   , 1, getAge   },
      { A_CHAR("hobby") , 1, getHobby },
};
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_3])

#if ALIB_VARIABLES

DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_1])
enum class PersonFormats
{
    Short,
    Default,
    All
};

ALIB_ENUMS_ASSIGN_RECORD( PersonFormats,  alib::variables::Declaration )

ALIB_RESOURCED(                PersonFormats,  alib::BASECAMP.GetResourcePool().Get(),
                                               alib::BASECAMP.ResourceCategory,    "PersonFormats" )
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_1])

DOX_MARKER( [DOX_STRINGS_PASSING_FUNC])
void foo(const String& string )
{
    (void) string; // ... do something with the string
}
DOX_MARKER( [DOX_STRINGS_PASSING_FUNC])


namespace std
{
    template<>
    struct hash<PersonFormats>
    {
        std::size_t operator()(PersonFormats src) const
        {
            return size_t( src );
        }
    };
}
#endif // ALIB_VARIABLES

namespace ut_aworx {

UT_CLASS

UT_METHOD( SimpleCodeSamples )
{
DOX_MARKER( [DOX_STRINGS_NULLED_AND_EMPTY])
String nulled(nullptr);       // constructs a nulled string
String empty( A_CHAR("") );   // constructs an empty but not nulled string

assert(  nulled.IsNull()    );
assert(  nulled.IsEmpty()   );
assert(  empty.IsNotNull()  );
assert(  empty.IsEmpty()    );

assert(  nulled != empty    );
DOX_MARKER( [DOX_STRINGS_NULLED_AND_EMPTY])

(void) empty;
(void) nulled;

DOX_MARKER( [DOX_ASTRING_NULLED])
// Default constructor does not allocate a buffer, yet. The instance is "nulled".
AString aString;
assert(  aString.IsNull()                 );
assert(  aString == NULL_STRING          );
assert(  aString.IsEmpty()                );
assert(  aString != EMPTY_STRING         );

// Append an empty string. This allocates a buffer. Now the AString is not nulled anymore.
aString << "";
assert(  aString.IsNotNull()              );
assert(  aString != NULL_STRING          );
assert(  aString.IsEmpty()                );
assert(  aString == EMPTY_STRING         );

// Append something.  Now the AString is not nulled and not empty.
aString << "ABC";
assert(  aString.IsNotNull()              );
assert(  aString != NULL_STRING          );
assert(  aString.IsNotEmpty()             );
assert(  aString != EMPTY_STRING         );

// Clear the contents
aString.Reset();
assert(  aString.IsNotNull()              );
assert(  aString != NULL_STRING          );
assert(  aString.IsEmpty()                );
assert(  aString == EMPTY_STRING         );

// Set nulled: disposes the allocated buffer. A seldom use case!
aString.SetNull();
assert(  aString.IsNull()                 );
assert(  aString == NULL_STRING          );
assert(  aString.IsEmpty()                );
assert(  aString != EMPTY_STRING         );
DOX_MARKER( [DOX_ASTRING_NULLED])

DOX_MARKER( [DOX_ASTRING_MODIFY_CONST_BUFFER])
const AString myAString( "Hello" );


// this would not compile because AString::SetCharAt() is not declared const....
//myAString.SetCharAt( 1, 'e' );

// ...but method VBuffer() is declared const.
myAString.VBuffer()[1]= 'e';
DOX_MARKER( [DOX_ASTRING_MODIFY_CONST_BUFFER])
}

#if ALIB_CAMP
UT_METHOD( Construction )
{
    UT_INIT()

DOX_MARKER( [DOX_STRINGS_PASSING])
// Passing a C++ string literal
foo( A_CHAR("/usr/bin") );

// Passing a std::string
std::basic_string<character> stdString( A_CHAR("/usr/bin") );
foo( stdString );

// Passing an AString
alib::AString aString( A_CHAR("/usr/bin") );
foo( aString );

// Passing a Substring
alib::Substring subString= aString.Substring(0, 4);
foo( subString );
DOX_MARKER( [DOX_STRINGS_PASSING])
}
#endif

UT_METHOD( PropertyFormatter )
{
    UT_INIT()
    UT_PRINT( "ALib PropertyFormatter tests and documentation sample" )
    {
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_4])

// Our data objects
Person p1= { A_CHAR("Sue") , 28, Hobbies::Hacking };
Person p2= { A_CHAR("John"), 35, Hobbies::Hacking };

// The format string. Make this changeable at run-time, e.g., load from INI-file!
String format= A_CHAR("{@name} is aged {@age} and his/her hobby is {@hobby}");

// create a formatter
alib::PropertyFormatter  propertyFormatter( format, PersonCallbacks  );

// format the two data objects
AString target;
{ ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    propertyFormatter.Format( target, p1 );
    target.NewLine();
    propertyFormatter.Format( target, p2 );
    target.NewLine();
}

// that's it!
std::cout << target;

DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_4])
    }
    ut.WriteResultFile( "DOX_STRINGS_PROPERTY_FORMATTER.txt", testOutputStream.str() , "");
    testOutputStream.str("");

    // do some more unit testing
    {
        bool caught= false;
        try
        {
            alib::PropertyFormatter  propertyFormatter( A_CHAR("Test {@Unknown}"), PersonCallbacks  );
        }
        catch( Exception& e )
        {
            UT_PRINT( "Exception caught as expected: " )
IF_ALIB_ALOX(
            LogTools::Exception( ut.lox, e, Verbosity::Info, ut.Domain, A_CHAR("  ") );  )

            if( e.Type() !=  format::FMTExceptions::UnknownPropertyInFormatString  )
            {
              UT_PRINT( "But wrong type: caught: {}, expected: {}",
                        e.Type(),
                         format::FMTExceptions::UnknownPropertyInFormatString )
              UT_TRUE( false )
            }

            caught= true;
        }
        if( !caught )
        {
            UT_PRINT( "No Exception caught. Expected: ",  format::FMTExceptions::UnknownPropertyInFormatString )
            UT_TRUE( caught )
        }

    }
}

#if ALIB_VARIABLES
UT_METHOD( PropertyFormatters )
{
    UT_INIT()
    UT_PRINT( "ALib PropertyFormatters tests and documentation sample" )


// we need to acquire the global allocator before calling BootstrapBulk here in unit tests
// In principle, this is completely forbidden...
{ ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_2])
alib::BASECAMP.GetResourcePool()->BootstrapBulk( alib::BASECAMP.ResourceCategory,

    // Enum records for enum class "PersonFormats"
    "PersonFormats",    A_CHAR( "0,FORMATS/SHORT"   ","  "S"  ","
                                "1,FORMATS/DEFAULT" ","  "S"  ","
                                "2,FORMATS/ALL"     ","  "S"       ),

    // Built-in default values for the variables
    "PersonFormats_D0", A_CHAR( "{@name}")                            ,
    "PersonFormats_D1", A_CHAR( "{@name} ({@age})")                   ,
    "PersonFormats_D2", A_CHAR( "{@name} aged {@age} loves {@hobby}" ),

    // Variable comments. These are written for example to an INI-file if the application
    // fetches default values at the end of the process. In this sample, all variables share
    // the same comment.
    "PersonFormats_C0", A_CHAR( "Short output format for lists of \"Persons\"."   ),
    "PersonFormats_C1", A_CHAR( "Default output format for lists of \"Persons\"." ),
    "PersonFormats_C2", A_CHAR( "Verbose output format for lists of \"Persons\"." ),

    nullptr // End marker für method BootstrapBulk (must be given!)
);
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_2])

DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_22])
alib::enumrecords::bootstrap::Bootstrap<PersonFormats>();
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_22])
}

ALIB_LOCK_WITH(BASECAMP.GetConfig())
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_31])
alib::PropertyFormatters<Person, PersonFormats> PersonFormatterMap( PersonCallbacks,
                                                                    *alib::BASECAMP.GetConfig() );
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_31])

DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_32])
using  FMTPerson= alib::format::PropertyFormatterMapAppendable<Person, PersonFormats>;
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_32])


DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_4])
// Our data objects
Person sue=  { A_CHAR("Sue") , 28, Hobbies::Hacking };
Person john= { A_CHAR("John"), 35, Hobbies::Hacking };

// format the two data objects
AString target;
target << FMTPerson( PersonFormatterMap, PersonFormats::Short, sue  ) << NEW_LINE;
target << FMTPerson( PersonFormatterMap, PersonFormats::All,   john ) << NEW_LINE;

std::cout << target;
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_4])
    ut.WriteResultFile( "DOX_STRINGS_PROPERTY_FORMATTER_MAP.txt", testOutputStream.str() ,"");
    testOutputStream.str("");
    target.Reset();
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_5])
// Define custom macros
#define FMT_PERSON(o,p)        FMTPerson( PersonFormatterMap, o                     , p )

#define FMT_PERSON_DEFAULT(p)  FMTPerson( PersonFormatterMap, PersonFormats::Default, p )
#define FMT_PERSON_SHORT(p)    FMTPerson( PersonFormatterMap, PersonFormats::Short  , p )
#define FMT_PERSON_ALL(p)      FMTPerson( PersonFormatterMap, PersonFormats::All    , p )

// Using the macros
target << FMT_PERSON( PersonFormats::Short, sue )     << NEW_LINE;

target << FMT_PERSON_SHORT( sue )                     << NEW_LINE;
target << FMT_PERSON_DEFAULT( sue )                   << NEW_LINE;
target << FMT_PERSON_ALL( sue )                       << NEW_LINE;

std::cout << target;
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_5])

  ut.WriteResultFile( "DOX_STRINGS_PROPERTY_FORMATTER_MAP_2.txt", testOutputStream.str() ,"");
  testOutputStream.str("");
    target.Reset();

DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_6])
ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( FMTPerson* )
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_6])


DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_7])
{ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    Formatter::Default->Format( target,
                                "The person is: {}",
                                FMT_PERSON_DEFAULT( john ) );
}
std::cout << target << endl;
DOX_MARKER( [DOX_STRINGS_PROPERTY_FORMATTER_MAP_7])
  ut.WriteResultFile( "DOX_STRINGS_PROPERTY_FORMATTER_MAP_3.txt", testOutputStream.str() ,"");
  testOutputStream.str("");
  target.Reset();
}
#endif

UT_METHOD( StdStrings )
{
    UT_INIT()
#if __has_include(<format>)
{
// use string types in std::format
DOX_MARKER( [DOX_STRINGS_STDFORMAT_ARGS])
NAString as("trams");
as.Reverse();
std::string s= std::format("Reversed: {}", as );
DOX_MARKER( [DOX_STRINGS_STDFORMAT_ARGS])
UT_EQ("Reversed: smart", s)
}
{
DOX_MARKER( [DOX_STRINGS_STDFORMAT_ARGS_FMT])
constexpr NSubstring fmt("The result is {}");
std::string s= std::format(fmt, 42 );
DOX_MARKER( [DOX_STRINGS_STDFORMAT_ARGS_FMT])
UT_EQ("The result is 42", s)
}

// append std::format result to AString
{
DOX_MARKER( [DOX_STRINGS_STDFORMAT_APPEND_NAIVE])
NAString as;
as << std::format("The result is {}", 42 );
DOX_MARKER( [DOX_STRINGS_STDFORMAT_APPEND_NAIVE])
UT_EQ(NString("The result is 42"), as)
}
{
DOX_MARKER( [DOX_STRINGS_STDFORMAT_APPEND_STDFORMAT])
NAString as;
as << NStdFormat("The result is {}", 42 );
DOX_MARKER( [DOX_STRINGS_STDFORMAT_APPEND_STDFORMAT])
UT_EQ(NString("The result is 42"), as )
}

// Use TAppendable in std::format
{
DOX_MARKER( [DOX_STRINGS_STDFORMAT_APPENDABLE])
std::string s=  std::format("Printing ALib Enum Record: {}", alib::Appendable(lang::Alignment::Center) );
DOX_MARKER( [DOX_STRINGS_STDFORMAT_APPENDABLE])
UT_EQ("Printing ALib Enum Record: Center", s)
}
#else
{
// use string types in std::format
NAString as("trams");
as.Reverse();
std::string s= fmt::format("Reversed: {}", as );
UT_EQ("Reversed: smart", s)
}
//{
//constexpr NSubstring fmt("The result is {}");
//std::string s= fmt::format(fmt, 42 );
//UT_EQ("The result is 42", s)
//}

// append std::format result to AString
{
NAString as;
as << fmt::format("The result is {}", 42 );
UT_EQ(NString("The result is 42"), as)
}
//{
//NAString as;
//as << NStdFormat("The result is {}", 42 );
//UT_EQ(NString("The result is 42"), as )
//}

// Use TAppendable in std::format
{
std::string s=  fmt::format("Printing ALib Enum Record: {}", alib::strings::Appendable(lang::Alignment::Center) );
UT_EQ("Printing ALib Enum Record: Center", s)
}
#endif

}


#include "aworx_unittests_end.hpp"

} //namespace

#endif //  ALIB_UT_DOCS && ALIB_STRINGS

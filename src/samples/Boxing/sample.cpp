// #################################################################################################
//  ALib C++ Library
//  Boxing Sample
//
//  Copyright 2019 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/compatibility/std_characters.hpp"
#include "alib/boxing/boxing.hpp"

#include <iostream>
#include <iomanip>
#include <limits>

using namespace std;
using namespace aworx;


// method that accepts any type of object, reference, pointer, fundamental type,....
void AcceptAny( const Box&  box );
void AcceptAny( const Box&  box )
{
    cout << std::setprecision(  std::numeric_limits<long double>::digits10 );

    const char* t= "  Type: ";
    const char* v= "  Value: ";

    // fundamental types
         if( box.IsType<bool             >() ) { cout << t << "bool        "; cout << v << (box.Unbox<bool             >() ? "true" : "false" ); }

#if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    else if( box.IsType<aworx::integer   >() ) { cout << t << "integer   "; cout << v <<  box.Unbox<aworx::integer >(); }
    else if( box.IsType<aworx::uinteger  >() ) { cout << t << "uinteger  "; cout << v <<  box.Unbox<aworx::uinteger>(); }
#else
    // If the non-bijective integral/float type boxing has been disabled in the library built, then a lot of more
    // types have to be fetched!
    else if( box.IsType<          int8_t >() ) { cout << t << "          int8_t"; cout << v <<  static_cast<int>( box.Unbox<int8_t  >()); }
    else if( box.IsType<         int16_t >() ) { cout << t << "         int16_t"; cout << v <<  box.Unbox<        int16_t >(); }
    else if( box.IsType<         int32_t >() ) { cout << t << "         int32_t"; cout << v <<  box.Unbox<        int32_t >(); }
    else if( box.IsType<         int64_t >() ) { cout << t << "         int64_t"; cout << v <<  box.Unbox<        int64_t >(); }
    else if( box.IsType< aworx::intGap_t >() ) { cout << t << " aworx::intGap_t"; cout << v <<  box.Unbox< aworx::intGap_t>(); }
    else if( box.IsType<        uint8_t  >() ) { cout << t << "         uint8_t"; cout << v <<  static_cast<int>(box.Unbox<       uint8_t  >()); }
    else if( box.IsType<        uint16_t >() ) { cout << t << "        uint16_t"; cout << v <<  box.Unbox<       uint16_t >(); }
    else if( box.IsType<        uint32_t >() ) { cout << t << "        uint32_t"; cout << v <<  box.Unbox<       uint32_t >(); }
    else if( box.IsType<        uint64_t >() ) { cout << t << "        uint64_t"; cout << v <<  box.Unbox<       uint64_t >(); }
    else if( box.IsType<aworx::uintGap_t >() ) { cout << t << "aworx::uintGap_t"; cout << v <<  box.Unbox<aworx::uintGap_t>(); }
#endif

#if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    else if( box.IsType<float            >() ) { cout << t << "float       "; cout << v <<  box.Unbox<float         >(); }
#endif
    else if( box.IsType<double           >() ) { cout << t << "double      "; cout << v <<  box.Unbox<double        >(); }
    else if( box.IsType<long double      >() ) { cout << t << "long double "; cout << v <<  box.Unbox<long double   >(); }

#if !ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    else if( box.IsType<wchar            >() ) { cout << t << "char        "; cout << v; wcout <<  box.Unbox<wchar_t>(); }
#else
    else if( box.IsType<char             >() ) { cout << t << "char        "; cout << v <<  box.Unbox<char          >(); }
    else if( box.IsType<wchar_t          >() ) { cout << t << "wchar_t     "; cout << v; wcout <<  box.Unbox<wchar_t>(); }
#endif

    // character arrays
    else if( box.IsArrayOf<char          >() ) { cout << t << "char[]    ";  cout << v <<  box.Unbox<std::string_view    >(); }
    else if( box.IsArrayOf<wchar_t       >() ) { cout << t << "wchar_t[] "; wcout << v <<  box.Unbox<std::wstring_view   >(); }
    else
    {
        // in this sample, no other types are covered
        cout << "  Type not known in this sample. " << std::endl;
        #if ALIB_DEBUG
            cout << "  Dbg info: Type name: \"" << aworx::lib::DbgTypeDemangler( box.TypeID() ).Get() << "\""  <<  endl;
            cout << "  Note:     More convenient debug options are found when module ALib Strings is bundled with ALib Boxing!" << endl;
        #endif
    }

    cout << endl;
}

// method that accepts a list of arbitrary objects, values, pointers...
template <typename... T> void AcceptMany( T&&... args )
{
    // fetch the arguments into an array of boxes
    Boxes boxes;
    boxes.Add( std::forward<T>( args )... );

    // process one by one
    for( const Box& box : boxes )
        AcceptAny( box );
}

// a test type
class MyType {};

// test the methods above
int main()
{
    // bootstrap boxing: this is necessary as this sample compiles with single module boxing
    //                   only. With the full library, bootstrap of boxing is performed inherently.
    aworx::lib::boxing::Bootstrap();


    std::cout << "bool:" << std::endl;
    { bool      val= true;
        AcceptAny(val);
        AcceptAny(&val); }

    std::cout << std::endl << "integer types:" << std::endl;
    {         int8_t   val=  -1;    AcceptAny(val); AcceptAny(&val); }
    {         int16_t  val=  -2;    AcceptAny(val); AcceptAny(&val); }
    {         int32_t  val=  -3;    AcceptAny(val); AcceptAny(&val); }
    {         int64_t  val=  -4;    AcceptAny(val); AcceptAny(&val); }
    {  aworx::intGap_t val=  -5;    AcceptAny(val); AcceptAny(&val); }
    {        uint8_t   val=   1;    AcceptAny(val); AcceptAny(&val); }
    {        uint16_t  val=   2;    AcceptAny(val); AcceptAny(&val); }
    {        uint32_t  val=   3;    AcceptAny(val); AcceptAny(&val); }
    {        uint64_t  val=   4;    AcceptAny(val); AcceptAny(&val); }
    { aworx::uintGap_t val=   5;    AcceptAny(val); AcceptAny(&val); }


    std::cout << std::endl << "float/double/long double:" << std::endl;
    { float       val= 0.12345678901234567890f; AcceptAny(val); AcceptAny(&val); }
    { double      val= 0.12345678901234567890 ; AcceptAny(val); AcceptAny(&val); }
    { long double val= 0.12345678901234567890L; AcceptAny(val); AcceptAny(&val); }


    std::cout << std::endl
              << "Character types: Must not be passed as pointers!"
              << std::endl;
    char      c=    'a';            AcceptAny(c);
    wchar_t   wc=  L'\u03B1';       AcceptAny(wc);


    std::cout << std::endl << "...instead character pointer types get boxed to character arrays. Must be zero-terminated when unboxed!"
              << std::endl;

    const  char    *     cString=   "abc";                  AcceptAny(cString);
    const wchar_t  *    wCString=  L"\u03B1\u03B2\u03B3";   AcceptAny(wCString);

    std::cout << "  Note: Wide character output is probably broken. Would be fixed with using module ALib Strings"
              << std::endl;


    std::cout << std::endl << "A type not known to the method:" << std::endl;
    MyType myType;
    AcceptAny(myType);

    std::cout << std::endl << "Finally, pass a list of arbitrary objects:" << std::endl;
    AcceptMany( "Hello", 42, 3.1415 );

    return 0;
}


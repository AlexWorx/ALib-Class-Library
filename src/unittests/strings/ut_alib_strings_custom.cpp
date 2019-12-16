// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_STRINGS


#include "alib/alox.hpp"
#include "alib/strings/util/tokenizer.hpp"
#include "alib/strings/numberformat.hpp"
#include "alib/compatibility/std_characters.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>


#include "unittests/aworx_unittests.hpp"


#define TESTCLASSNAME       CPP_ALib_Strings_Custom

using namespace std;
using namespace aworx;

// #################################################################################################
// ####### String/CString construction from Implicit/Explicit/Mutable
// #################################################################################################
using namespace aworx::lib::strings;

template <typename TChar>
class MyStringImplicit
{
    private:
        TLocalString<TChar, 64> theString;

    public:
        ALIB_CPP14_CONSTEXPR MyStringImplicit(const char* modifier) { theString << "This is a " << modifier << " MyStringImplicit"; }
        constexpr  const TChar*     GetMyBuffer() const { return theString.Buffer();   }
                   integer          GetMyLength() const { return theString.Length(); }
};

template <typename TChar>
class MyStringExplicit
{
    private:
        TLocalString<TChar, 64> theString;

    public:
        ALIB_CPP14_CONSTEXPR MyStringExplicit(const char* modifier) { theString << "This is a " << modifier << " MyStringExplicit"; }
        constexpr  const TChar*     GetMyBuffer() const { return theString.Buffer();   }
                   integer          GetMyLength() const { return theString.Length(); }
};

template <typename TChar>
class MyStringMutable
{
    private:
        TLocalString<TChar, 64> theString;

    public:
        ALIB_CPP14_CONSTEXPR MyStringMutable(const char* modifier) { theString << "This is a " << modifier << " MyStringMutable"; }
        ALIB_CPP14_CONSTEXPR  const TChar*     GetMyBuffer()       { return theString.Buffer();   }
                              integer          GetMyLength()       { return theString.Length(); }
};

template <typename TChar>
class MyCStringImplicit
{
    private:
        TLocalString<TChar, 64> theString;

    public:
        ALIB_CPP14_CONSTEXPR MyCStringImplicit(const char* modifier) { theString << "This is a " << modifier << " MyCStringImplicit"; theString.Terminate(); }
        constexpr  const TChar*     GetMyBuffer() const { return theString.Buffer();   }
                   integer          GetMyLength() const { return theString.Length(); }
};

template <typename TChar>
class MyCStringExplicit
{
    private:
        TLocalString<TChar, 64> theString;

    public:
        ALIB_CPP14_CONSTEXPR MyCStringExplicit(const char* modifier) { theString << "This is a " << modifier << " MyCStringExplicit"; theString.Terminate(); }
        constexpr  const TChar*     GetMyBuffer() const { return theString.Buffer();   }
                   integer          GetMyLength() const { return theString.Length(); }
};

template <typename TChar>
class MyCStringMutable
{
    private:
        TLocalString<TChar, 64> theString;

    public:
        ALIB_CPP14_CONSTEXPR MyCStringMutable(const char* modifier) { theString << "This is a " << modifier << " MyCStringMutable"; theString.Terminate(); }
        ALIB_CPP14_CONSTEXPR const TChar*     GetMyBuffer()       { return theString.Buffer();   }
                             integer          GetMyLength()       { return theString.Length(); }
};


namespace aworx { namespace lib { namespace characters {

    ALIB_CHARACTER_ARRAY( MyStringImplicit<nchar>, nchar, Implicit              , NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( MyStringImplicit<nchar>, nchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( MyStringImplicit<nchar>, nchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY( MyStringExplicit<nchar>, nchar, ExplicitOnly , NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( MyStringExplicit<nchar>, nchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( MyStringExplicit<nchar>, nchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY( MyStringImplicit<wchar>, wchar, Implicit              , NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( MyStringImplicit<wchar>, wchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( MyStringImplicit<wchar>, wchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY( MyStringExplicit<wchar>, wchar, ExplicitOnly , NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( MyStringExplicit<wchar>, wchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( MyStringExplicit<wchar>, wchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY( MyStringImplicit<xchar>, xchar, Implicit              , NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( MyStringImplicit<xchar>, xchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( MyStringImplicit<xchar>, xchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY( MyStringExplicit<xchar>, xchar, ExplicitOnly , NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( MyStringExplicit<xchar>, xchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( MyStringExplicit<xchar>, xchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY_MUTABLE( MyStringMutable <nchar>, nchar, NONE)
    ALIB_CHARACTER_ARRAY_MUTABLE( MyStringMutable <wchar>, wchar, NONE)
    ALIB_CHARACTER_ARRAY_MUTABLE( MyStringMutable <xchar>, xchar, NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE( MyStringMutable<nchar> , nchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE( MyStringMutable<nchar> , nchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE( MyStringMutable<wchar> , wchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE( MyStringMutable<wchar> , wchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE( MyStringMutable<xchar> , xchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE( MyStringMutable<xchar> , xchar, return static_cast<integer>( src.GetMyLength() ); )

    ALIB_CHARACTER_ARRAY( MyCStringImplicit<nchar>, nchar, Implicit , NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( MyCStringImplicit<nchar>, nchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( MyCStringImplicit<nchar>, nchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY( MyCStringExplicit<nchar>, nchar, ExplicitOnly , NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( MyCStringExplicit<nchar>, nchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( MyCStringExplicit<nchar>, nchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY( MyCStringImplicit<wchar>, wchar, Implicit , NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( MyCStringImplicit<wchar>, wchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( MyCStringImplicit<wchar>, wchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY( MyCStringExplicit<wchar>, wchar, ExplicitOnly , NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( MyCStringExplicit<wchar>, wchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( MyCStringExplicit<wchar>, wchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY( MyCStringImplicit<xchar>, xchar, Implicit              , NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( MyCStringImplicit<xchar>, xchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( MyCStringImplicit<xchar>, xchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY( MyCStringExplicit<xchar>, xchar, ExplicitOnly , NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( MyCStringExplicit<xchar>, xchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( MyCStringExplicit<xchar>, xchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY_MUTABLE( MyCStringMutable <nchar>, nchar, NONE)
    ALIB_CHARACTER_ARRAY_MUTABLE( MyCStringMutable <wchar>, wchar, NONE)
    ALIB_CHARACTER_ARRAY_MUTABLE( MyCStringMutable <xchar>, xchar, NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE( MyCStringMutable<nchar> , nchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE( MyCStringMutable<nchar> , nchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE( MyCStringMutable<wchar> , wchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE( MyCStringMutable<wchar> , wchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE( MyCStringMutable<xchar> , xchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE( MyCStringMutable<xchar> , xchar, return static_cast<integer>( src.GetMyLength() ); )

    ALIB_CHARACTER_ZT_ARRAY( MyCStringImplicit<nchar>, nchar, Implicit, NONE)
    ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER( MyCStringImplicit<nchar>, nchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH( MyCStringImplicit<nchar>, nchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ZT_ARRAY( MyCStringExplicit<nchar>, nchar, ExplicitOnly, NONE)
    ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER( MyCStringExplicit<nchar>, nchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH( MyCStringExplicit<nchar>, nchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ZT_ARRAY( MyCStringImplicit<wchar>, wchar, Implicit, NONE)
    ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER( MyCStringImplicit<wchar>, wchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH( MyCStringImplicit<wchar>, wchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ZT_ARRAY( MyCStringExplicit<wchar>, wchar, ExplicitOnly, NONE)
    ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER( MyCStringExplicit<wchar>, wchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH( MyCStringExplicit<wchar>, wchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ZT_ARRAY( MyCStringImplicit<xchar>, xchar, Implicit, NONE)
    ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER( MyCStringImplicit<xchar>, xchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH( MyCStringImplicit<xchar>, xchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ZT_ARRAY( MyCStringExplicit<xchar>, xchar, ExplicitOnly, NONE)
    ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER( MyCStringExplicit<xchar>, xchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH( MyCStringExplicit<xchar>, xchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ZT_ARRAY_MUTABLE( MyCStringMutable <nchar>, nchar, NONE)
    ALIB_CHARACTER_ZT_ARRAY_MUTABLE( MyCStringMutable <wchar>, wchar, NONE)
    ALIB_CHARACTER_ZT_ARRAY_MUTABLE( MyCStringMutable <xchar>, xchar, NONE)
    ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER_MUTABLE( MyCStringMutable<nchar> , nchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH_MUTABLE( MyCStringMutable<nchar> , nchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER_MUTABLE( MyCStringMutable<wchar> , wchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH_MUTABLE( MyCStringMutable<wchar> , wchar, return static_cast<integer>( src.GetMyLength() ); )
    ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER_MUTABLE( MyCStringMutable<xchar> , xchar, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH_MUTABLE( MyCStringMutable<xchar> , xchar, return static_cast<integer>( src.GetMyLength() ); )

}}} // namespace [aworx::lib::characters]

//### preps for String
template<typename TChar>
void printString( const TString<TChar>& str, const NCString& comment= nullptr )
{
    cout << " String<"                  <<
    #if ALIB_DEBUG
        lib::DbgTypeDemangler( typeid(TChar) ).Get()
    #else
        ( std::is_same<TChar,nchar>:: value ? "nchar" : std::is_same<TChar,wchar>::value ? "wchar" : "xchar" )
    #endif

         << "> was accepted: Value= \"" << NString256(str)
         << "\" "                       << comment << endl;

}

extern char fakeObject;
char fakeObject;
template<typename TChar, typename T, typename EnableIf= void>  struct t_accepts_S_implicit : public  std::false_type {};
template<typename TChar, typename T, typename EnableIf= void>  struct t_accepts_S_explicit : public  std::false_type {};
template<typename TChar, typename T> struct t_accepts_S_implicit<TChar, T, typename std::enable_if<std::is_same<decltype(printString<TChar>(                  std::declval<T>()) ),void>::value>::type> : public  std::true_type {};
template<typename TChar, typename T> struct t_accepts_S_explicit<TChar, T, typename std::enable_if<std::is_same<TString<TChar>, decltype(TString<TChar>(*reinterpret_cast<T*>(&fakeObject)))>::value>::type> : public  std::true_type {};


template<typename TChar, typename T, typename EnableIf=
typename std::enable_if<!t_accepts_S_implicit<TChar,T>::value>::type>   void accept_S_implicit( T&         )
{
    cout << " String<"                                   <<
    #if ALIB_DEBUG
        lib::DbgTypeDemangler( typeid(TChar) ).Get()
         << "> not accepted: Cant construct from type <" << lib::DbgTypeDemangler( typeid(T)     ).Get()
         << "> implicitly "
    #else
        ( std::is_same<TChar,nchar>:: value ? "nchar" : std::is_same<TChar,wchar>::value ? "wchar" : "xchar" )
         << "> not accepted with custom type."
    #endif
         << endl;

}
template<typename TChar, typename T>
typename std::enable_if< t_accepts_S_implicit<TChar,T>::value>::type         accept_S_implicit( T& object  )
{
    printString<TChar>(        object ,   "implicitly invoked" );
}

template<typename TChar, typename T, typename EnableIf=
typename std::enable_if<!t_accepts_S_explicit<TChar,T>::value>::type>   void accept_S_explicit( T&         )
{
    cout << " String<"               <<
    #if ALIB_DEBUG
        lib::DbgTypeDemangler( typeid(TChar) ).Get()
         << "> not accepted: Type <" << lib::DbgTypeDemangler( typeid(T)     ).Get()
         << "> is not allowed for explicit construction"
    #else
        ( std::is_same<TChar,nchar>:: value ? "nchar" : std::is_same<TChar,wchar>::value ? "wchar" : "xchar" )
         << "> not accepted: Custom type is not allowed for explicit construction"
    #endif
         << endl;
}
template<typename TChar, typename T>
typename std::enable_if< t_accepts_S_explicit<TChar,T>::value>::type         accept_S_explicit( T& object  )
{
    printString<TChar>( TString<TChar>(object),   "explicitly invoked" );
}


//### preps for CString
template<typename TChar>
void printCString( const TCString<TChar>& str, const NCString& comment= nullptr )
{
    cout << "CString<"                  <<
    #if ALIB_DEBUG
        lib::DbgTypeDemangler( typeid(TChar) ).Get()
    #else
        ( std::is_same<TChar,nchar>:: value ? "nchar" : std::is_same<TChar,wchar>::value ? "wchar" : "xchar" )
    #endif
         << "> was accepted: Value= \"" << NString256(str)
         << "\" "                       << comment << endl;

}

template<typename TChar, typename T, typename EnableIf= void>  struct t_accepts_CS_implicit : public  std::false_type {};
template<typename TChar, typename T>                           struct t_accepts_CS_implicit<TChar, T, typename std::enable_if<

    std::is_same<decltype(printCString<TChar>(                   std::declval<T>()) ),void>::value

>::type> : public  std::true_type {};


template<typename TChar, typename T, typename EnableIf= void>  struct t_accepts_CS_explicit : public  std::false_type {};
template<typename TChar, typename T>                           struct t_accepts_CS_explicit<TChar, T, typename std::enable_if<

     lib::characters::T_ZTCharArray<ATMP_RCVP(T),TChar>::Access != lib::characters::AccessType::ExplicitOnly
  &&  std::is_same<TCString<TChar>, decltype(TCString<TChar>(*reinterpret_cast<T*>(&fakeObject)))  >::value

>::type> : public  std::true_type {};

template<typename TChar, typename T>                           struct t_accepts_CS_explicit<TChar, T, typename std::enable_if<

     lib::characters::T_ZTCharArray<ATMP_RCVP(T),TChar>::Access == lib::characters::AccessType::ExplicitOnly
  && std::is_same<TCString<TChar>, decltype(TCString<TChar>(*reinterpret_cast<T*>(&fakeObject) ))>::value

>::type> : public  std::true_type {};


template<typename TChar, typename T, typename EnableIf= typename std::enable_if<
!t_accepts_CS_implicit<TChar, T>::value
                                                                                >::type>
void accept_CS_implicit( T&  )
{
    cout << "CString<"                                   <<
         #if ALIB_DEBUG
         lib::DbgTypeDemangler( typeid(TChar) ).Get()
         << "> not accepted: Cant construct from type <" << lib::DbgTypeDemangler( typeid(T)     ).Get()
         << "> implicitly "
         #else
         ( std::is_same<TChar,nchar>:: value ? "nchar" : std::is_same<TChar,wchar>::value ? "wchar" : "xchar" )
         << "> not accepted with custom type."
         #endif
         << endl;

}
template<typename TChar, typename T> typename std::enable_if<
t_accepts_CS_implicit<TChar, T>::value
                                                                                >::type
accept_CS_implicit( T& object )
{
    printCString<TChar>(        object ,   "implicitly invoked" );
}

template<typename TChar, typename T, typename EnableIf= typename std::enable_if<
!t_accepts_CS_explicit<TChar, T>::value
                                                                                >::type>
void accept_CS_explicit( T&         )
{
    cout << "CString<"               <<
         #if ALIB_DEBUG
         lib::DbgTypeDemangler( typeid(TChar) ).Get()
         << "> not accepted: Type <" << lib::DbgTypeDemangler( typeid(T)     ).Get()
         << "> is not allowed for explicit construction"
         #else
         ( std::is_same<TChar,nchar>:: value ? "nchar" : std::is_same<TChar,wchar>::value ? "wchar" : "xchar" )
         << "> not accepted: Custom type is not allowed for explicit construction"
         #endif
         << endl;
}
template<typename TChar, typename T> typename std::enable_if<
    t_accepts_CS_explicit<TChar, T>::value
&&  (lib::characters::T_ZTCharArray<ATMP_RCVP(T),TChar>::Access == lib::characters::AccessType::ExplicitOnly)
                                                                                >::type
accept_CS_explicit( T& object  )
{
    printCString<TChar>( TCString<TChar>(object),   "explicitly invoked" );
}

template<typename TChar, typename T> typename std::enable_if<
    t_accepts_CS_explicit<TChar, T>::value
&&  (lib::characters::T_ZTCharArray<ATMP_RCVP(T),TChar>::Access != lib::characters::AccessType::ExplicitOnly)
                                                                                >::type
accept_CS_explicit( T& object  )
{
    printCString<TChar>( TCString<TChar>(object                                                 ),   "explicitly invoked" );
}



template<typename TChar, typename T>
void testAccept( ut_aworx::AWorxUnitTesting& ut,
                 T& object, bool  SImplicitAllowed, bool  SExplicitAllowed,
                            bool CSImplicitAllowed, bool CSExplicitAllowed )
{
    UT_EQ( SImplicitAllowed,  t_accepts_S_implicit<TChar ALIB_COMMA T>::value )  accept_S_implicit<TChar>( object );
    UT_EQ( SExplicitAllowed,  t_accepts_S_explicit<TChar ALIB_COMMA T>::value )  accept_S_explicit<TChar>( object );
    UT_EQ(CSImplicitAllowed, t_accepts_CS_implicit<TChar ALIB_COMMA T>::value ) accept_CS_implicit<TChar>( object );
    UT_EQ(CSExplicitAllowed, t_accepts_CS_explicit<TChar ALIB_COMMA T>::value ) accept_CS_explicit<TChar>( object );
}

template<typename TChar, typename T>
void testAccept( ut_aworx::AWorxUnitTesting& ut,
                 T* object, bool  SImplicitAllowed, bool  SExplicitAllowed,
                            bool CSImplicitAllowed, bool CSExplicitAllowed )
{
    UT_EQ( SImplicitAllowed,  t_accepts_S_implicit<TChar ALIB_COMMA T>::value )  accept_S_implicit<TChar>( object );
    UT_EQ( SExplicitAllowed,  t_accepts_S_explicit<TChar ALIB_COMMA T>::value )  accept_S_explicit<TChar>( object );
    UT_EQ(CSImplicitAllowed, t_accepts_CS_implicit<TChar ALIB_COMMA T>::value ) accept_CS_implicit<TChar>( object );
    UT_EQ(CSExplicitAllowed, t_accepts_CS_explicit<TChar ALIB_COMMA T>::value ) accept_CS_explicit<TChar>( object );
}



// #################################################################################################
// ####### String/CString construction from Implicit/Explicit/Mutable
// #################################################################################################

namespace ut_aworx {

UT_CLASS()

//--------------------------------------------------------------------------------------------------
//--- CustomTypes
//--------------------------------------------------------------------------------------------------
UT_METHOD( CustomTypes )
{
    UT_INIT()

    UT_PRINT( "\nImplicit constructible String (nchar):" )
          MyStringImplicit<nchar>    mysNImplicit("mutable");
    testAccept<nchar>(ut,            mysNImplicit       , true , true , false, false );
    testAccept<nchar>(ut,           &mysNImplicit       , true , true , false, false );

    UT_PRINT( "\nImplicit constructible String (wchar):" )
          MyStringImplicit<wchar>    mysWImplicit("mutable");
    testAccept<wchar>(ut,            mysWImplicit       , true , true , false, false );
    testAccept<wchar>(ut,           &mysWImplicit       , true , true , false, false );

    UT_PRINT( "\nImplicit constructible String (xchar):" )
          MyStringImplicit<xchar>    mysXImplicit("mutable");
    testAccept<xchar>(ut,            mysXImplicit       , true , true , false, false );
    testAccept<xchar>(ut,           &mysXImplicit       , true , true , false, false );

    UT_PRINT( "\nImplicit constructible String (nchar) const:" )
    const MyStringImplicit<nchar>    mysConstNImplicit("const");
    testAccept<nchar>(ut,            mysConstNImplicit  , true , true , false, false );
    testAccept<nchar>(ut,           &mysConstNImplicit  , true , true , false, false );

    UT_PRINT( "\nImplicit constructible String (wchar) const:" )
    const MyStringImplicit<wchar>    mysConstWImplicit("const");
    testAccept<wchar>(ut,            mysConstWImplicit  , true , true , false, false );
    testAccept<wchar>(ut,           &mysConstWImplicit  , true , true , false, false );

    UT_PRINT( "\nImplicit constructible String (xchar) const:" )
    const MyStringImplicit<xchar>    mysConstXImplicit("const");
    testAccept<xchar>(ut,            mysConstXImplicit  , true , true , false, false );
    testAccept<xchar>(ut,           &mysConstXImplicit  , true , true , false, false );


    //--- Explicit ----
    UT_PRINT( "\nExplicit constructible String (nchar):" )
          MyStringExplicit<nchar>    mysNExplicit("mutable");
    testAccept<nchar>(ut,            mysNExplicit       , false , true , false, false );
    testAccept<nchar>(ut,           &mysNExplicit       , false , true , false, false );

    UT_PRINT( "\nExplicit constructible String (wchar):" )
          MyStringExplicit<wchar>    mysWExplicit("mutable");
    testAccept<wchar>(ut,            mysWExplicit       , false , true , false, false );
    testAccept<wchar>(ut,           &mysWExplicit       , false , true , false, false );

    UT_PRINT( "\nExplicit constructible String (xchar):" )
          MyStringExplicit<xchar>    mysXExplicit("mutable");
    testAccept<xchar>(ut,            mysXExplicit       , false , true , false, false );
    testAccept<xchar>(ut,           &mysXExplicit       , false , true , false, false );

    UT_PRINT( "\nExplicit constructible String (nchar) const:" )
    const MyStringExplicit<nchar>    mysConstNExplicit("const");
    testAccept<nchar>(ut,            mysConstNExplicit  , false , true , false, false );
    testAccept<nchar>(ut,           &mysConstNExplicit  , false , true , false, false );

    UT_PRINT( "\nExplicit constructible String (wchar) const:" )
    const MyStringExplicit<wchar>    mysConstWExplicit("const");
    testAccept<wchar>(ut,            mysConstWExplicit  , false , true , false, false );
    testAccept<wchar>(ut,           &mysConstWExplicit  , false , true , false, false );

    UT_PRINT( "\nExplicit constructible String (xchar) const:" )
    const MyStringExplicit<xchar>    mysConstXExplicit("const");
    testAccept<xchar>(ut,            mysConstXExplicit  , false , true , false, false );
    testAccept<xchar>(ut,           &mysConstXExplicit  , false , true , false, false );


    //--- Mutable Only ----
    UT_PRINT( "\nMutable constructible String (nchar):" )
          MyStringMutable<nchar>    mysNMutable("mutable");
    testAccept<nchar>(ut,            mysNMutable       , false , true , false, false );
    testAccept<nchar>(ut,           &mysNMutable       , false , true , false, false );

    UT_PRINT( "\nMutable constructible String (wchar):" )
          MyStringMutable<wchar>    mysWMutable("mutable");
    testAccept<wchar>(ut,            mysWMutable       , false , true , false, false );
    testAccept<wchar>(ut,           &mysWMutable       , false , true , false, false );

    UT_PRINT( "\nMutable constructible String (xchar):" )
          MyStringMutable<xchar>    mysXMutable("mutable");
    testAccept<xchar>(ut,            mysXMutable       , false , true , false, false );
    testAccept<xchar>(ut,           &mysXMutable       , false , true , false, false );

    UT_PRINT( "\nMutable constructible String (nchar) const:" )
    const MyStringMutable<nchar>    mysConstNMutable("const");
    testAccept<nchar>(ut,            mysConstNMutable  , false , false , false, false );
    testAccept<nchar>(ut,           &mysConstNMutable  , false , false , false, false );

    UT_PRINT( "\nMutable constructible String (wchar) const:" )
    const MyStringMutable<wchar>    mysConstWMutable("const");
    testAccept<wchar>(ut,            mysConstWMutable  , false , false , false, false );
    testAccept<wchar>(ut,           &mysConstWMutable  , false , false , false, false );

    UT_PRINT( "\nMutable constructible String (xchar) const:" )
    const MyStringMutable<xchar>    mysConstXMutable("const");
    testAccept<xchar>(ut,            mysConstXMutable  , false , false , false, false );
    testAccept<xchar>(ut,           &mysConstXMutable  , false , false , false, false );


    //---------------------------------- CStrings -----------------------------------
    //--- Implicit ----
    UT_PRINT( "\nImplicit constructible CString (nchar):" )
          MyCStringImplicit<nchar>   myCsNImplicit("mutable");
    testAccept<nchar>(ut,            myCsNImplicit       , true , true , true, true );
    testAccept<nchar>(ut,           &myCsNImplicit       , true , true , true, true );

    UT_PRINT( "\nImplicit constructible CString (wchar):" )
          MyCStringImplicit<wchar>   myCsWImplicit("mutable");
    testAccept<wchar>(ut,            myCsWImplicit       , true , true , true, true );
    testAccept<wchar>(ut,           &myCsWImplicit       , true , true , true, true );

    UT_PRINT( "\nImplicit constructible CString (xchar):" )
          MyCStringImplicit<xchar>   myCsXImplicit("mutable");
    testAccept<xchar>(ut,            myCsXImplicit       , true , true , true, true );
    testAccept<xchar>(ut,           &myCsXImplicit       , true , true , true, true );

    UT_PRINT( "\nImplicit constructible CString (nchar) const:" )
    const MyCStringImplicit<nchar>   myCsConstNImplicit("const");
    testAccept<nchar>(ut,            myCsConstNImplicit  , true , true , true, true );
    testAccept<nchar>(ut,           &myCsConstNImplicit  , true , true , true, true );

    UT_PRINT( "\nImplicit constructible CString (wchar) const:" )
    const MyCStringImplicit<wchar>   myCsConstWImplicit("const");
    testAccept<wchar>(ut,            myCsConstWImplicit  , true , true , true, true );
    testAccept<wchar>(ut,           &myCsConstWImplicit  , true , true , true, true );

    UT_PRINT( "\nImplicit constructible CString (xchar) const:" )
    const MyCStringImplicit<xchar>   myCsConstXImplicit("const");
    testAccept<xchar>(ut,            myCsConstXImplicit  , true , true , true, true );
    testAccept<xchar>(ut,           &myCsConstXImplicit  , true , true , true, true );


    //--- Explicit ----
    UT_PRINT( "\nExplicit constructible CString (nchar):" )
          MyCStringExplicit<nchar>   myCsNExplicit("mutable");
    testAccept<nchar>(ut,            myCsNExplicit       , false , true , false, true );
    testAccept<nchar>(ut,           &myCsNExplicit       , false , true , false, true );

    UT_PRINT( "\nExplicit constructible CString (wchar):" )
          MyCStringExplicit<wchar>   myCsWExplicit("mutable");
    testAccept<wchar>(ut,            myCsWExplicit       , false , true , false, true );
    testAccept<wchar>(ut,           &myCsWExplicit       , false , true , false, true );

    UT_PRINT( "\nExplicit constructible CString (xchar):" )
          MyCStringExplicit<xchar>   myCsXExplicit("mutable");
    testAccept<xchar>(ut,            myCsXExplicit       , false , true , false, true );
    testAccept<xchar>(ut,           &myCsXExplicit       , false , true , false, true );

    UT_PRINT( "\nExplicit constructible CString (nchar) const:" )
    const MyCStringExplicit<nchar>   myCsConstNExplicit("const");
    testAccept<nchar>(ut,            myCsConstNExplicit  , false , true , false, true );
    testAccept<nchar>(ut,           &myCsConstNExplicit  , false , true , false, true );

    UT_PRINT( "\nExplicit constructible CString (wchar) const:" )
    const MyCStringExplicit<wchar>   myCsConstWExplicit("const");
    testAccept<wchar>(ut,            myCsConstWExplicit  , false , true , false, true );
    testAccept<wchar>(ut,           &myCsConstWExplicit  , false , true , false, true );

    UT_PRINT( "\nExplicit constructible CString (xchar) const:" )
    const MyCStringExplicit<xchar>   myCsConstXExplicit("const");
    testAccept<xchar>(ut,            myCsConstXExplicit  , false , true , false, true );
    testAccept<xchar>(ut,           &myCsConstXExplicit  , false , true , false, true );


    //--- Mutable Only ----
    UT_PRINT( "\nMutable constructible CString (nchar):" )
          MyCStringMutable<nchar>   myCsNMutable("mutable");
    testAccept<nchar>(ut,            myCsNMutable       , false , true , false, true );
    testAccept<nchar>(ut,           &myCsNMutable       , false , true , false, true );

    UT_PRINT( "\nMutable constructible CString (wchar):" )
          MyCStringMutable<wchar>   myCsWMutable("mutable");
    testAccept<wchar>(ut,            myCsWMutable       , false , true , false, true );
    testAccept<wchar>(ut,           &myCsWMutable       , false , true , false, true );

    UT_PRINT( "\nMutable constructible CString (xchar):" )
          MyCStringMutable<xchar>   myCsXMutable("mutable");
    testAccept<xchar>(ut,            myCsXMutable       , false , true , false, true );
    testAccept<xchar>(ut,           &myCsXMutable       , false , true , false, true );


    UT_PRINT( "\nMutable constructible CString (nchar) const:" )
    const MyCStringMutable<nchar>   myCsConstNMutable("const");
    testAccept<nchar>(ut,            myCsConstNMutable  , false , false , false, false );
    testAccept<nchar>(ut,           &myCsConstNMutable  , false , false , false, false );

    UT_PRINT( "\nMutable constructible CString (wchar) const:" )
    const MyCStringMutable<wchar>   myCsConstWMutable("const");
    testAccept<wchar>(ut,            myCsConstWMutable  , false , false , false, false );
    testAccept<wchar>(ut,           &myCsConstWMutable  , false , false , false, false );

    UT_PRINT( "\nMutable constructible CString (xchar) const:" )
    const MyCStringMutable<xchar>   myCsConstXMutable("const");
    testAccept<xchar>(ut,            myCsConstXMutable  , false , false , false, false );
    testAccept<xchar>(ut,           &myCsConstXMutable  , false , false , false, false );
}


#include "unittests/aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_STRINGS

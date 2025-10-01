// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/variables/variables.prepro.hpp"
#include "ALib.Strings.Vector.H"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Variables;
    import ALib.EnumRecords.Bootstrap;
#else
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Variables.H"
#endif
// ======================================   Implementation   =======================================

namespace alib::variables {
DOX_MARKER([DOX_ENUMS_RECORD_PARSER])
void Declaration::Parse()
{
    enumrecords::bootstrap::EnumRecordParser::Get( ERSerializable::EnumElementName  );           // field from parent class
                                             ERSerializable::MinimumRecognitionLength= 0;  // omit reading, but fix to zero
    enumrecords::bootstrap::EnumRecordParser::Get( typeName                            , true ); // last field!
}
DOX_MARKER([DOX_ENUMS_RECORD_PARSER])
}

#if !DOXYGEN
namespace alib::variables::detail {

void  VMeta_Bool     ::imPort(VDATA* data, Configuration& cfg, const StringEscaper&    , const String& src )  { auto result= cfg.ParseBooleanToken(src); data->As<Bool>()= result.first; data->As<Bool>().TokenIndex= result.second; }
void  VMeta_integer  ::imPort(VDATA* data, Configuration& cfg, const StringEscaper&    , const String& src )  { data->As<integer>()= integer(Substring(src).TrimStart().ParseInt  (0, &cfg.NumberFormat)); }
void  VMeta_float    ::imPort(VDATA* data, Configuration& cfg, const StringEscaper&    , const String& src )  { data->As<double >()=         Substring(src).TrimStart().ParseFloat(0, &cfg.NumberFormat) ; }
void  VMeta_String   ::imPort(VDATA* data, Configuration&    , const StringEscaper& esc, const String& src )  { String2K buf; esc.Unescape(src, buf); data->As<AStringPA>().Reset(buf); }

void  VMeta_Bool     ::exPort(VDATA* data, Configuration& cfg, const StringEscaper& esc, AString& dest)  { String64 buf; esc.Escape(cfg.WriteBooleanToken( data->As<Bool>().Value, data->As<Bool>().TokenIndex, buf ), dest, A_CHAR("\0")); }
void  VMeta_integer  ::exPort(VDATA* data, Configuration&    , const StringEscaper&    , AString& dest)  { dest << data->As<integer>(); }
void  VMeta_float    ::exPort(VDATA* data, Configuration&    , const StringEscaper&    , AString& dest)  { dest << data->As<double >(); }
void  VMeta_String   ::exPort(VDATA* data, Configuration&    , const StringEscaper& esc, AString& dest)  { esc.Escape(data->As<AStringPA>()        , dest, A_CHAR("\0")); }
void  VMeta_Box      ::exPort(VDATA* data, Configuration&    , const StringEscaper& esc, AString& dest)  { esc.Escape(String1K() << data->As<Box>(), dest, A_CHAR("\0")); }
void  VMeta_Box      ::imPort(VDATA* data, Configuration&    , const StringEscaper& esc, const String& src )
{
    Substring parser;
    String2K buf;
    esc.Unescape(src, buf );
    parser= buf;

    // try integral
    {
        integer val;
        if(parser.ConsumeInt(val) && parser.Trim().IsEmpty())
        {
            data->As<Box>()= val;
            return;
        }
        parser= src;
    }

    // try float
    {
        double val;
        if(parser.ConsumeFloat(val) && parser.Trim().IsEmpty())
        {
            data->As<Box>()= val;
            return;
        }
        parser= src;
    }

    // we just store the source string
    data->As<Box>()= src;
}

namespace {
void imPortStringVector(VDATA* data, const StringEscaper& esc, const String& src, const String& delimiter )
{
    LocalAllocator4K la;
    StringVectorMA results(la);
    esc.UnescapeTokens(results, src, delimiter);
    auto& dataSA= data->As<StringVectorPA>();
    dataSA.clear();
    for( auto& it : results )
    {
        Substring value= it;
        value.ConsumeChar('\r');
        value.ConsumeChar('\n');
        value.ConsumeCharFromEnd('\n');
        value.ConsumeCharFromEnd('\r');
        dataSA.Add( value );
    }
}

void  exPortStringVector(VDATA* data, const StringEscaper& esc, AString& dest, const String& delimiter )
{
    auto& dataSA= data->As<StringVectorPA>();
    if( !dataSA.empty())
        for( auto it= dataSA.begin() ;;  )
        {
            esc.Escape(*it, dest, delimiter);

            if( ++it == dataSA.end() )
                break;

            dest << delimiter;
            dest << NEW_LINE;
        }
}

}
void  VMeta_StringVectorComma::imPort(VDATA* data, Configuration&    , const StringEscaper& esc, const String& src )
{
    imPortStringVector(data, esc, src, A_CHAR(","));
}

void  VMeta_StringVectorComma::exPort(VDATA* data, Configuration&    , const StringEscaper& esc, AString& dest)
{
    exPortStringVector(data, esc, dest, A_CHAR(","));
}

void  VMeta_StringVectorSemicolon::imPort(VDATA* data, Configuration&, const StringEscaper& esc, const String& src )
{
    imPortStringVector(data, esc, src, A_CHAR(";"));
}

void  VMeta_StringVectorSemicolon::exPort(VDATA* data, Configuration&, const StringEscaper& esc, AString& dest)
{
    exPortStringVector(data, esc, dest,A_CHAR(";"));
}

} // namespace [alib::variables]
#endif // if !DOXYGEN

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
#include <cmath>
#include <cstring>
#include "alib/boxing/boxing.prepro.hpp"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Format;
    import   ALib.Lang;
    import   ALib.Time;
    import   ALib.Strings;
    import   ALib.Strings.Calendar;
    import   ALib.Boxing;
#  if ALIB_SYSTEM
    import   ALib.System;
#  endif
#if ALIB_CAMP
    import   ALib.Camp.Base;
#  endif
#else
#   include "ALib.Lang.H"
#   include "ALib.Time.H"
#   include "ALib.Strings.H"
#   include "ALib.Strings.Calendar.H"
#   include "ALib.Boxing.H"
#   include "ALib.System.H"
#   include "ALib.Format.H"
#   include "ALib.Camp.Base.H"
#endif
// ======================================   Implementation   =======================================
namespace   alib::format {

void FFormat_CallerInfo( const Box& box, const String& formatSpec, NumberFormat&, AString& target )
{
    FMTCallerInfo fmtCI{*box.Unbox<CallerInfo*>() };
    fmtCI.Format( formatSpec.IsNotEmpty() ? formatSpec
                                    #if ALIB_CAMP
                                          : BASECAMP.GetResource("FMTCI")
                                    #else
                                          : A_CHAR( "[@ sf:sl from ''ya'' by ''ta'']")
                                    #endif
                 , target );
}

AString& FMTCallerInfo::Format( Substring format, AString& target, lang::CurrentData targetData ) const
{
    if ( targetData == lang::CurrentData::Clear )
        target.Reset();

    // this ensures that target is not nulled, as all other appends are NC-versions
    target._("");

    while ( format.IsNotEmpty() )
    {
        // read n equal characters
        int   n=  1;
        character c=  format.ConsumeChar();
        while ( format.ConsumeChar(c) )
            ++n;

        switch (c)
        {
            case '\'': // single quotes
            {
                // one or more pairs of single quotes?
                if ( n > 1 )
                {
                    int pairs= n / 2;
                    target.InsertChars<NC>( '\'', pairs );
                    n-= (pairs * 2);
                }

                // one single quote?
                if ( n == 1 )
                {
                    // search end
                    integer end= format.IndexOf( '\'' );
                    if ( end < 1 )
                    {
                        ALIB_WARNING( "ALIB", "Format Error: Missing single Quote" )
                        target <<     "Format Error: Missing closing single quote character <'>" ;
                        return target;
                    }

                    target._<NC>( format, 0, end );
                    format.ConsumeChars<NC>( end + 1 );
                }

            } break;

            // source information
            case 's':
            {
                if ( n == 1 )
                {
                    if ( format.ConsumeChar('f') )
                    {
                        if( ci.File ) target._<NC>( ci.File );
                        else          target._<NC>( "<NULL>" );
                        break;
                    }
                    if ( format.ConsumeChar('l') )
                    {
                        if( ci.File )
                            target._<NC>( ci.Line );
                        break;
                    }
                    if ( format.ConsumeChar('m') )
                    {
                        if( ci.File )
                            target._<NC>( ci.Func );
                        break;
                    }
                }
                // otherwise: copy what was in
                target.InsertChars<NC>( c, n );
                break;
            }

        #if !ALIB_SINGLE_THREADED
            // thread information
            case 't':
            {
                if ( n == 1 )
                {
                    Thread* thread= Thread::Get(ci.ThreadID);

                    // alib thread name
                    if ( format.ConsumeChar('n') )
                    {
                        target._<NC>( thread ? thread->GetName()
                                    #if ALIB_CAMP
                                             : BASECAMP.GetResource("FMTCINT")
                                    #else
                                             : A_CHAR( "<None>" )
                                    #endif
                                    );
                         break;
                    }
                    // alib thread id
                    if ( format.ConsumeChar('i') )
                    {
                        if ( thread ) target._<NC>( thread->GetID() );
                    #if ALIB_CAMP
                        else          target._<NC>( BASECAMP.GetResource("FMTCINR")  );
                    #else
                        else          target._<NC>( A_CHAR("<Null>")  );
                    #endif
                        break;
                    }

                    // native thread id
                    if ( format.ConsumeChar('c') )
                    {
                        if constexpr ( sizeof(std::thread::id) == sizeof(uint16_t) )
                        {
                            uint16_t nativeID= 0;
                            memcpy(&nativeID, &ci.ThreadID, 2);
                            target._<NC>("0x")._<NC>(Hex(nativeID, 4));
                        }
                        if constexpr ( sizeof(std::thread::id) == sizeof(uint32_t) )
                        {
                            uint32_t nativeID= 0;
                            memcpy(&nativeID, &ci.ThreadID, 4);
                            target._<NC>("0x")._<NC>(Hex(nativeID, 8));
                        }
                        if constexpr ( sizeof(std::thread::id) == sizeof(uint64_t) )
                        {
                            uint64_t nativeID= 0;
                            memcpy(&nativeID, &ci.ThreadID, 8);
                            target._<NC>("0x")._<NC>(Hex(nativeID, 16));
                        }

                        break;
                    }

                    // all/auto
                    if ( format.ConsumeChar('a') )
                    {
                        target._<NC>( ci.ThreadID );
                        break;
                    }
                }
                // otherwise: copy what was in
                target.InsertChars<NC>( c, n );
                break;
            }
        #endif // !ALIB_SINGLE_THREADED

            // type information
            case 'y':
            {
                if ( n == 1 )
                {
                    // full type name
                    if ( format.ConsumeChar('f') )
                    {
                      #if !ALIB_DEBUG
                        target._<NC>( BASECAMP.GetResource("FMTCINY") );
                      #else
                        if (!ci.TypeInfo) target._<NC>(
                                    #if ALIB_CAMP
                                             BASECAMP.GetResource("FMTCINY")
                                    #else
                                             A_CHAR( "<None>" )
                                    #endif
                                                );
                        else              target << lang::DbgTypeDemangler(*ci.TypeInfo).Get();
                      #endif
                        break;
                    }

                    // stripped type name
                    if ( format.ConsumeChar('n') )
                    {
                      #if !ALIB_DEBUG
                        target._<NC>( BASECAMP.GetResource("FMTCINY") );
                      #else
                        if (!ci.TypeInfo)
                            target._<NC>(
                                    #if ALIB_CAMP
                                            BASECAMP.GetResource("FMTCINY")
                                    #else
                                            A_CHAR( "<None>" )
                                    #endif
                                );
                        else
                        {
                            NString2K typeName(*ci.TypeInfo);
                            target._<NC>( typeName );
                        }
                      #endif
                        break;
                    }

                    // automatic type together with shortened function name
                    if ( format.ConsumeChar('a') )
                    {
                      #if !ALIB_DEBUG
                        target._<NC>( ci.Func )._<NC>("()");
                      #else
                        if (ci.TypeInfo)
                            target._<NC>( *ci.TypeInfo )._<NC>("::");
                        target._<NC>( ci.Func )._<NC>("()");
                      #endif
                        break;
                    }

                }
                // otherwise: copy what was in
                target.InsertChars<NC>( c, n );
                break;
            }

            default: // otherwise: copy what was in
                target.InsertChars<NC>( c, n );
                break;
        }

    }

    return target;
}


// #################################################################################################
// FFormat_DateTime
// #################################################################################################
#if !DOXYGEN
DOX_MARKER([DOX_BOXING_IFORMAT_DATETIME])
void FFormat_DateTime( const Box& box, const String& formatSpec, NumberFormat&, AString& target )
{
    strings::util::CalendarDateTime tct( box.Unbox<DateTime>() );
    tct.Format( formatSpec.IsNotEmpty() ? formatSpec
                                    #if ALIB_CAMP
                                        : BASECAMP.GetResource("FMTDT"),
                                    #else
                                        : A_CHAR( "yyyy-MM-dd HH:mm:ss" ),
                                    #endif
                target );
}
DOX_MARKER([DOX_BOXING_IFORMAT_DATETIME])
#endif


// #################################################################################################
// format::ByteSize
// #################################################################################################
#if !DOXYGEN

NumberFormat* BYTESIZE_NUMBER_FORMAT= nullptr;

namespace {

double   convertTo(uinteger val, ByteSizeUnits unit)
{
    double v= double(val);
    int loopEnd;
    double divisor;
    if( unit < ByteSizeUnits::IEC_END)
    {
        loopEnd= int(unit);
        divisor= double(1024);
    }
    else
    {
        loopEnd= int(unit - ByteSizeUnits::SI);
        divisor= double(1000);
    }

    for (int i = 0; i < loopEnd; ++i)
            v/= divisor;
    return v;
}

ALIB_WARNINGS_IGNORE_NOTHING_RETURNED
std::pair<double,ByteSizeUnits>     getMagnitude( uinteger val, uinteger byteLimit, uinteger factor)
{
    ByteSizeUnits unit= ByteSizeUnits(0);
    if( val < byteLimit )
        return std::make_pair( double(val), ByteSizeUnits(0) );

    while(val != 0)
    {
        ++unit;
        if( val < byteLimit * factor)
            return std::make_pair( double(val) / double(factor), unit );

        val/= factor;
    }
}
ALIB_WARNINGS_RESTORE
} // namespace alib::format[::anonymous]

void FormatByteSize(AString& target, uinteger val, uint16_t magnitudeThreshold,
                    char unitSeparator, ByteSizeUnits unit, NumberFormat& nf)
{
    target.EnsureRemainingCapacity(128);

    auto magnitude= (unit ==  ByteSizeUnits::IEC) ? getMagnitude(val, magnitudeThreshold, 1024ull)
                                                  : getMagnitude(val, magnitudeThreshold, 1000ull);

    if( magnitude.second ==  ByteSizeUnits::IEC )
        target << Dec( val, nullptr );
    else
        target.SetLength( strings::detail::WriteFloat( magnitude.first,
                                                       target.VBuffer(), target.Length(), 0, nf)  );

    if( unitSeparator )
        target << unitSeparator;
    target <<  (magnitude.second + unit);
} // FormatByteSize()

std::pair<double, ByteSizeUnits>     ByteSizeIEC::GetMagnitude()
{
    auto result= getMagnitude( Value, MagnitudeThreshold, 1024 );
    result.second+= ByteSizeUnits::IEC;
    return result;
}

std::pair<double,ByteSizeUnits>     ByteSizeSI::GetMagnitude()
{
    auto result= getMagnitude( Value, MagnitudeThreshold, 1000 );
    result.second+= ByteSizeUnits::SI;
    return result;
}

double   ByteSizeIEC::ConvertTo(ByteSizeUnits unit) { return convertTo(Value, unit); };
double   ByteSizeSI ::ConvertTo(ByteSizeUnits unit) { return convertTo(Value, unit); };


} namespace alib::strings {

void AppendableTraits< format::ByteSizeSI, character, lang::HeapAllocator>::operator()( AString& target, const  format::ByteSizeSI src )
{
     format::FormatByteSize( target, src.Value, src.MagnitudeThreshold, src.UnitSeparator,
                           format::ByteSizeUnits::SI, * format::BYTESIZE_NUMBER_FORMAT          );
}
void AppendableTraits< format::ByteSizeIEC, character, lang::HeapAllocator>::operator()( AString& target, const  format::ByteSizeIEC src )
{
     format::FormatByteSize( target, src.Value, src.MagnitudeThreshold, src.UnitSeparator,
                           format::ByteSizeUnits::IEC, * format::BYTESIZE_NUMBER_FORMAT         );
}

} // namespace [alib::strings]

ALIB_BOXING_VTABLE_DEFINE( alib::format::ByteSizeIEC      , vt_lang_format_bytesize_iec )
ALIB_BOXING_VTABLE_DEFINE( alib::format::ByteSizeSI       , vt_lang_format_bytesize_si  )
ALIB_BOXING_VTABLE_DEFINE( alib::format::ByteSizeUnits    , vt_lang_format_bytesize_units )


#endif  //!DOXYGEN



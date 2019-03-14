// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_STRINGFORMAT_STRINGFORMAT)
#   include "alib/stringformat/stringformat.hpp"
#endif

#if !defined (HPP_ALIB_LIB_ALIBMODULES)
#   include "alib/lib/alibmodules.hpp"
#endif

#if !defined (HPP_ALIB_RESULTS_RESULTS)
#    include "alib/results/results.hpp"
#endif

#if !defined (HPP_ALIB_STRINGFORMAT_FORMATTER)
#   include "alib/stringformat/formatter.hpp"
#endif

#if !defined (HPP_ALIB_STRINGFORMAT_FORMATTER_PYTHONSTYLE)
#   include "alib/stringformat/formatterpythonstyle.hpp"
#endif

#if !defined (HPP_ALIB_STRINGFORMAT_FORMATTER_JAVASTYLE)
#   include "alib/stringformat/formatterjavastyle.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_UTIL_TOKEN)
#   include "alib/strings/util/token.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#    include "alib/resources/resources.hpp"
#endif

#if ALIB_MODULE_SYSTEM && !defined(HPP_ALIB_SYSTEM_CALENDAR)
#   include "alib/system/calendar.hpp"
#endif

#if ALIB_MODULE_THREADS && !defined(HPP_ALIB_THREADS_THREAD)
#   include "alib/threads/thread.hpp"
#endif


using namespace aworx::lib::strings;

ALIB_BOXING_VTABLE_DEFINE( aworx::lib::stringformat::Exceptions   , vt_stringformat_exceptions )

namespace aworx { namespace lib {

stringformat::Stringformat STRINGFORMAT;

/**
 * This \alibmod implements string formatting based on "placeholder format strings" similar to
 * good old \c printf function and similar to what is given with many modern programming language's
 * core libraries.
 *
 * Please consult \ref alib_mod_stringformat "ALib Module Stringformat - Programmer's Manual"
 * for further information.
 */
namespace stringformat {


Stringformat::Stringformat()
: Module( ALIB_VERSION, ALIB_REVISION, "ALIB_STRFMT" )
{
    ALIB_ASSERT_ERROR( this == &STRINGFORMAT,
     "Instances of class Stringformat must not be created. Use singleton aworx::lib::STRINGFORMAT" )
}


void Stringformat::init( InitLevels level, int, const char**, const wchar_t** )
{
    if( level == InitLevels::PrepareResources )
    {
        ALIB.CheckDistribution();
        ALIB_BOXING_VTABLE_REGISTER( vt_stringformat_exceptions )


DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum3] )
Resources->AddBulk( ResourceCategory,

    "FmtExceptionsPrefix"         ,      A_CHAR("format::") ,
    "FmtExceptionsPostfix"        ,      A_CHAR("")         ,

    "FmtExceptions"               ,

        // general formatter errors
        A_CHAR("11,ArgumentIndexIs0"                       ",FMG11,"
               "12,ArgumentIndexOutOfBounds"               ",FMG12,"
               "13,IncompatibleTypeCode"                   ",FMG13,"

               // Formatter Python Style
               "101,MissingClosingBracket"                 ",FPS101,"
               //...
               //...
DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum3] )
               "102,MissingPrecisionValuePS"               ",FPS102,"
               "103,DuplicateTypeCode"                     ",FPS103,"
               "104,UnknownTypeCode"                       ",FPS104,"
               "105,ExclamationMarkExpected"               ",FPS105,"
               "106,UnknownConversionPS"                   ",FPS106,"
               "107,PrecisionSpecificationWithInteger"     ",FPS107,"
               "108,MissingReplacementStrings"             ",FPS108,"

               // Formatter Java Style
               "201,NegativeValuesInBracketsNotSupported"  ",FJS101,"
               "202,MissingPrecisionValueJS"               ",FJS102,"
               "203,HexadecimalFloatFormatNotSupported"    ",FJS103,"
               "204,NoAlternateFormOfConversion"           ",FJS104,"
               "205,NoPrecisionWithConversion"             ",FJS105,"
               "206,UnknownDateTimeConversionSuffix"       ",FJS106,"
               "207,UnknownConversionJS"                   ",FJS107,"

               // PropertyFormatter and PropertyFormatters
               "501,UnknownPropertyInFormatString"         ",PFM501,"
               "502,ErrorInResultingFormatString"          ",PFM502,"
               "510,MissingConfigurationVariable"          ",PFM510,"

               // class Text
               "601,UnknownMarker"                         ",ST601,"
               "602,EndmarkerWithoutStart"                 ",ST602"     ),

    // general formatter errors
DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum3b] )
               //...
               //...
               //...
    "FMG11"      , A_CHAR( "Argument index 0 not allowed.\\n"
                           "In: {!Q}\\n     {!Fill}^" ),
    "FMG12"      , A_CHAR( "Argument index #{} is out of bounds. {} arguments are available.\\n"
                           "In: {!Q}\\n     {!Fill}^" ),
    "FMG13"      , A_CHAR( "Incompatible type code '{}' for argument #{} which is of type {!Q}.\\n"
                           "In: {!Q}\\n     {!Fill}^" ),

    // Formatter Python Style
    "FPS101"     , A_CHAR( "Closing bracket '}}' of placeholder not found (or syntax error).\\n"
                           "In: {!Q}\\n     {!Fill}^" ),
    //...
    //...
    //...
DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum3b] )
    "FPS102"     , A_CHAR( "Missing precision value after '.' character."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FPS103"     , A_CHAR( "Duplicate type code '{}' given (previous was '{}')."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FPS104"     , A_CHAR( "Unknownw type code '{}' given."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FPS105"     , A_CHAR( "Excpected '!' in continuation of placeholder."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FPS106"     , A_CHAR( "Unknown conversion \"!{}\"."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FPS107"     , A_CHAR( "Precision not allowed with integer format."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FPS107"     , A_CHAR( "Missing pair of replacement strings \")<src><repl>\" after \"!Replace\"."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),


    // Formatter Java Style
    "FJS101"     , A_CHAR( "Brackets for negative values not implemented/supported."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FJS102"     , A_CHAR( "Missing precision value after '.' character."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FJS103"     , A_CHAR( "Hexadecimal float format not implemented/supported."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FJS104"     , A_CHAR( "Alternate form '#' not supported with conversion {!Q}."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FJS105"     , A_CHAR( "Precision specification (\".{}\") is not supported with conversion {!Q}."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FJS106"     , A_CHAR( "Unknown date/time conversion suffix '{}'."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FJS107"     , A_CHAR( "Unknown conversion character '{}'."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),


        // PropertyFormatter and PropertyFormatters
    "PFM501"     , A_CHAR( "Unknown property identifier \"{}{}\" in format string.\\n"
                           "Format string: {!Q}.\\n"
                           "Allowed identifiers: "                                                                ),
    "PFM502"     , A_CHAR( "Error in resulting format string.\\nSource format: {!Q}.")                             ,
    "PFM510"     , A_CHAR( "Missing or empty configuration variable {!Q} containing user defined format string." ),

        // class Text
    "ST601"     ,  A_CHAR( "Unknown marker at position {}.\\n"
                           "  Marked text: {!Q}.\\n"
                           "                {!Fill}^\\n"                                                          ),
    "ST602"     ,  A_CHAR( "Found an end-marker without a start marker at position {}.\\n"
                           "  Marked text: {!Q}.\\n"
                           "                {!Fill}^\\n"                                                          ),

    // Append(DateTime::Duration)
      "TS_ZERO" , A_CHAR("zero time")
    , "DayP"    , A_CHAR(" days")
    , "DayS"    , A_CHAR(" day")
    , "HourP"   , A_CHAR(" hours")
    , "HourS"   , A_CHAR(" hour")
    , "MinP"    , A_CHAR(" minutes")
    , "MinS"    , A_CHAR(" minute")
    , "SecP"    , A_CHAR(" seconds")
    , "SecS"    , A_CHAR(" second")
    , "MlSecP"  , A_CHAR(" ms")
    , "MlSecS"  , A_CHAR(" ms")
    , "McSecP"  , A_CHAR(" \u00B5s")
    , "McSecS"  , A_CHAR(" \u00B5s")
    , "NSecP"   , A_CHAR(" ns")
    , "NSecS"   , A_CHAR(" ns")       ,

// end of AddBulk()
nullptr );

            ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( stringformat::Exceptions )
            #if ALIB_MODULE_TIME
                #if ALIB_MODULE_SYSTEM
                    lib::boxing::Register<FFormat, lib::boxing::TMappedTo<time::DateTime>>(FFormat_DateTime);
                #endif
                ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N( time::DateTime::Duration )
                ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W( time::DateTime::Duration )
                ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X( time::DateTime::Duration )
                ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N( time::Ticks   ::Duration )
                ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W( time::Ticks   ::Duration )
                ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X( time::Ticks   ::Duration )
            #endif


        // create formatter. This has to be done very early, so that they can be used during
        // initialization.
        defaultFormatter.reset( new FormatterPythonStyle() );
        defaultFormatter->Next.reset( new FormatterJavaStyle() )   ;

    } // level == InitLevels::PrepareResources

}

void Stringformat::terminationCleanUp()
{
    ALIB_ASSERT_WARNING( defaultFormatter.use_count() == 1,
                         "defaultFormatter still saved somewhere else." )
    defaultFormatter.reset();
}



#if ALIB_MODULE_TIME && ALIB_MODULE_SYSTEM && !ALIB_DOCUMENTATION_PARSER
DOX_MARKER([DOX_ALIB_BOXING_IFORMAT_DATETIME])
void FFormat_DateTime( const Box& box, const String& formatSpec, AString& target )
{
    system::CalendarDateTime tct( box.Unbox<DateTime>() );
    tct.Format( formatSpec, target );
}
DOX_MARKER([DOX_ALIB_BOXING_IFORMAT_DATETIME])
#endif

} namespace strings {

// #################################################################################################
// T_Append<DateTime::Duration>
// #################################################################################################
#if ALIB_MODULE_TIME  && !ALIB_DOCUMENTATION_PARSER
namespace {

    template<typename TChar>
    void appendDateTime(TAString<TChar>& target, const DateTime::Duration pSrc)
    {
        auto& module= STRINGFORMAT;
        using Duration= DateTime::Duration;
        Duration src= pSrc;
        auto nanos= src.InNanoseconds();
        if( nanos == 0 )
        {
            target << module.GetResource( "TS_ZERO" );
            return;
        }

        if( nanos < 0 )
        {
            target << A_CHAR("- ");
            src= Duration() - src;
        }

        TNumberFormat<TChar> nf( TNumberFormat<TChar>::Global );
        nf.FractionalPartWidth= 2;
        int64_t v= src.InAbsoluteDays();
        if( v >= 10 )
        {
            target << TFormat<TChar>( src.InDays(), &nf ) << module.GetResource( "DayP"   );
            return;
        }

        if( v >  0 )
        {
            target << v << ( v != 1 ?  module.GetResource( "DayP" )
                                    :  module.GetResource( "DayS" ) );

            Duration cpy= src - ( Duration::FromAbsoluteDays(v) );

            target << ' ' << TFormat<TChar>( cpy.InHours(), &nf ) << module.GetResource( "HourP"   );
            return;
        }

        v= src.InAbsoluteHours();
        if( v >  0 )
        {
            target << v << ( v != 1 ?  module.GetResource( "HourP" )
                                    :  module.GetResource( "HourS" )  );

            Duration cpy= src - ( Duration::FromAbsoluteHours(v) );

            auto minutes= cpy.InAbsoluteMinutes();
            target << ' ' << minutes <<  (minutes!= 1 ?  module.GetResource( "MinP" )
                                                      :  module.GetResource( "MinS" ) );
            return;
        }

        v= src.InAbsoluteMinutes();
        if( v > 0 )
        {
            target << v << ( v != 1 ?  module.GetResource( "MinP" )
                                    :  module.GetResource( "MinS" ) );

            Duration cpy= src - ( Duration::FromAbsoluteMinutes(v) );

            auto seconds= cpy.InAbsoluteSeconds();
            target << ' ' << seconds <<  (seconds!= 1 ?  module.GetResource( "SecP" )
                                                      :  module.GetResource( "SecS" ) );
            return;
        }

        v= src.InAbsoluteSeconds();
        if( v > 0 )
        {
            target << TFormat<TChar>( src.InSeconds(), &nf ) << module.GetResource( "SecP"  );
            return;
        }

        nf.DecMinimumFieldWidth= 3;

        auto val= src.InAbsoluteMilliseconds();
        if( val >= 1 )
        {
            target << TFormat<TChar>(val,&nf) << ( val!= 1  ?  module.GetResource( "MlSecP" )
                                                            :  module.GetResource( "MlSecS" ) );
            return;
        }

        val= src.InAbsoluteMicroseconds();
        if( val >= 1 )
        {
            target << TFormat<TChar>(val,&nf) << ( val!= 1  ?  module.GetResource( "McSecP" )
                                                            :  module.GetResource( "McSecS" ) );
            return;
        }

        val= src.InNanoseconds();
        target << TFormat<TChar>(val,&nf) << ( val!= 1  ?  module.GetResource( "NSecP" )
                                                        :  module.GetResource( "NSecS" ) );
        return;
    }
} //anonymous namespace

template<typename TChar>
void T_Append<time::DateTime::Duration,TChar>::operator()( TAString<TChar>& target, const time::DateTime::Duration src )
{
    appendDateTime( target, src );
}

template<typename TChar>
void T_Append<time::Ticks::Duration,TChar>::operator()( TAString<TChar>& target, const time::Ticks::Duration src)
{
    // simply convert the ticks-duration to a DateTime duration and use its append function
    appendDateTime( target, time::DateTime::Duration::FromNanoseconds( src.InNanoseconds() ));
}

// Instantiations
template void T_Append<time::DateTime::Duration, nchar>::operator()( TAString<nchar>&, const time::DateTime::Duration   );
template void T_Append<time::DateTime::Duration, wchar>::operator()( TAString<wchar>&, const time::DateTime::Duration   );
template void T_Append<time::DateTime::Duration, xchar>::operator()( TAString<xchar>&, const time::DateTime::Duration   );
template void T_Append<time::Ticks::   Duration, nchar>::operator()( TAString<nchar>&, const time::Ticks   ::Duration   );
template void T_Append<time::Ticks::   Duration, wchar>::operator()( TAString<wchar>&, const time::Ticks   ::Duration   );
template void T_Append<time::Ticks::   Duration, xchar>::operator()( TAString<xchar>&, const time::Ticks   ::Duration   );


#endif //ALIB_MODULE_TIME

}}}// namespace [aworx::lib::strings]

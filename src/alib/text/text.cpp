// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_TEXT_TEXT)
#      include "alib/text/text.hpp"
#   endif
#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#   if !defined (HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#      include "alib/enums/recordbootstrap.hpp"
#   endif
#   if ALIB_THREADS && !defined(HPP_ALIB_THREADS_THREAD)
#      include "alib/threads/thread.hpp"
#   endif
#   if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#      include "alib/resources/resources.hpp"
#   endif

#   if !defined (HPP_ALIB_TEXT_FORMATTER_PYTHONSTYLE)
#      include "alib/text/formatterpythonstyle.hpp"
#   endif
#   if !defined (HPP_ALIB_TEXT_FORMATTER_JAVASTYLE)
#      include "alib/text/formatterjavastyle.hpp"
#   endif

#   if ALIB_SYSTEM && !defined(HPP_ALIB_SYSTEM_CALENDAR)
#      include "alib/system/calendar.hpp"
#   endif
#   if !defined (HPP_ALIB_FS_MODULES_DISTRIBUTION)
#      include "alib/lib/fs_modules/distribution.hpp"
#   endif
#endif // !defined(ALIB_DOX)


using namespace aworx::lib::strings;

ALIB_BOXING_VTABLE_DEFINE( aworx::lib::text::Exceptions, vt_text_exceptions )

namespace aworx { namespace lib {

text::Text TEXT;

/**
 * This \alibmod implements string formatting based on "placeholder format strings" similar to
 * good old \c printf function and similar to what is given with many modern programming language's
 * core libraries.
 *
 * Please consult \ref alib_mod_text "ALib Module Text - Programmer's Manual"
 * for further information.
 */
namespace text {


Text::Text()
: Module( ALIB_VERSION, ALIB_REVISION, "TEXT" )
{
    ALIB_ASSERT_ERROR( this == &TEXT,
     "Instances of class Text must not be created. Use singleton aworx::lib::TEXT" )
}


void Text::bootstrap( BootstrapPhases phase, int, const char**, const wchar_t** )
{
    if( phase == BootstrapPhases::PrepareResources )
    {
        ALIB.CheckDistribution();
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_text_exceptions )


#if !ALIB_RESOURCES_OMIT_DEFAULTS
resourcePool->BootstrapBulk( ResourceCategory,

    "E<", A_CHAR("format::"),

// general formatter errors

    "E0" , A_CHAR( "11,ArgumentIndexIs0"                       ",FMG11"   ),
    "E1" , A_CHAR( "12,ArgumentIndexOutOfBounds"               ",FMG12"   ),
    "E2" , A_CHAR( "13,IncompatibleTypeCode"                   ",FMG13"   ),

    // Formatter Python Style
    "E3" , A_CHAR( "101,MissingClosingBracket"                 ",FPS101"  ),
    //...
    //...
    "E4" , A_CHAR( "102,MissingPrecisionValuePS"               ",FPS102"  ),
    "E5" , A_CHAR( "103,DuplicateTypeCode"                     ",FPS103"  ),
    "E6" , A_CHAR( "104,UnknownTypeCode"                       ",FPS104"  ),
    "E7" , A_CHAR( "105,ExclamationMarkExpected"               ",FPS105"  ),
    "E8" , A_CHAR( "106,UnknownConversionPS"                   ",FPS106"  ),
    "E9" , A_CHAR( "107,PrecisionSpecificationWithInteger"     ",FPS107"  ),
    "E10", A_CHAR( "108,MissingReplacementStrings"            ",FPS108"  ),

    // Formatter Java Style
    "E11" , A_CHAR( "201,NegativeValuesInBracketsNotSupported" ",FJS101"  ),
    "E12" , A_CHAR( "202,MissingPrecisionValueJS"              ",FJS102"  ),
    "E13" , A_CHAR( "203,HexadecimalFloatFormatNotSupported"   ",FJS103"  ),
    "E14" , A_CHAR( "204,NoAlternateFormOfConversion"          ",FJS104"  ),
    "E15" , A_CHAR( "205,NoPrecisionWithConversion"            ",FJS105"  ),
    "E16" , A_CHAR( "206,UnknownDateTimeConversionSuffix"      ",FJS106"  ),
    "E17" , A_CHAR( "207,UnknownConversionJS"                  ",FJS107"  ),

    // PropertyFormatter and PropertyFormatters
    "E18" , A_CHAR( "501,UnknownPropertyInFormatString"        ",PFM501"  ),
    "E19" , A_CHAR( "502,ErrorInResultingFormatString"         ",PFM502"  ),
    "E20" , A_CHAR( "510,MissingConfigurationVariable"         ",PFM510"  ),

    // class Paragraphs
    "E21" , A_CHAR( "601,UnknownMarker"                        ",ST601"   ),
    "E22" , A_CHAR( "602,EndmarkerWithoutStart"                ",ST602"   ),

    // general formatter errors
    "FMG11"      , A_CHAR( "Argument index 0 not allowed.\\n"
                           "In: {!Q}\\n     {!Fill}^" ),
    "FMG12"      , A_CHAR( "Argument index #{} is out of bounds. {} arguments are available.\\n"
                           "In: {!Q}\\n     {!Fill}^" ),
    "FMG13"      , A_CHAR( "Incompatible type code {!Q'} for argument #{} which is of type {!Q}.\\n"
                           "In: {!Q}\\n     {!Fill}^" ),

    // Formatter Python Style
    "FPS101"     , A_CHAR( "Closing bracket '}}' of placeholder not found (or syntax error).\\n"
                           "In: {!Q}\\n     {!Fill}^" ),
    //...
    //...
    //...
    "FPS102"     , A_CHAR( "Missing precision value after '.' character."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FPS103"     , A_CHAR( "Duplicate type code {!Q'} given (previous was {!Q'})."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FPS104"     , A_CHAR( "Unknownw type code {!Q'} given."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FPS105"     , A_CHAR( "Excpected '!' in continuation of placeholder."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FPS106"     , A_CHAR( "Unknown conversion \"!{}\"."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FPS107"     , A_CHAR( "Precision not allowed with integer format."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FPS108"     , A_CHAR( "Missing pair of replacement strings \")<src><repl>\" after \"!Replace\"."
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
    "FJS106"     , A_CHAR( "Unknown date/time conversion suffix {!Q'}."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    "FJS107"     , A_CHAR( "Unknown conversion character {!Q'}."
                           "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),


    // PropertyFormatter and PropertyFormatters
    "PFM501"     , A_CHAR( "Unknown property identifier \"{}{}\" in format string.\\n"
                           "Format string: {!Q}.\\n"
                           "Allowed identifiers: "                                                               ),
    "PFM502"     , A_CHAR( "Error in resulting format string.\\nSource format: {!Q}."                            ),
    "PFM510"     , A_CHAR( "Missing or empty configuration variable {!Q} containing user defined format string." ),

    // class Paragraphs
    "ST601"     ,  A_CHAR( "Unknown marker at position {}.\\n"
                           "  Marked text: {!Q}.\\n"
                           "                {!Fill}^\\n"                                                         ),
    "ST602"     ,  A_CHAR( "Found an end-marker without a start marker at position {}.\\n"
                           "  Marked text: {!Q}.\\n"
                           "                {!Fill}^\\n"                                                         ),

// end of BootstrapBulk()
nullptr );
#endif // !ALIB_RESOURCES_OMIT_DEFAULTS


            ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( text::Exceptions )


        // create formatters. This has to be done very early, so that they can be used during
        // initialization.
        Formatter::defaultFormatter.reset( new FormatterPythonStyle() );
        Formatter::defaultFormatter->Next.reset( new FormatterJavaStyle() )   ;

    } // phase == BootstrapPhases::PrepareResources

    else if( phase == BootstrapPhases::PrepareConfig )
    {
        EnumRecords<Exceptions>::Bootstrap();
    }

}

void Text::shutdown( ShutdownPhases phase )
{
    if( phase != ShutdownPhases::Destruct )
        return;

    ALIB_ASSERT_WARNING( Formatter::defaultFormatter.use_count() == 1,
                         "DefaultFormatter still shared more than once: ",
                         Formatter::defaultFormatter.use_count() )

    Formatter::defaultFormatter.reset();
}




}}} // namespace [aworx::lib::text]

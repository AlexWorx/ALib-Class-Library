// #################################################################################################
//  alib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/alox/textlogger/variables.hpp"
#   include "alib/monomem/localallocator.hpp"
#   include "alib/monomem/aliases/stringvector.hpp"
#   include "alib/strings/util/tokenizer.hpp"
#   include "alib/enums/serialization.hpp"
#endif // !DOXYGEN


#if !DOXYGEN

// FormatMetaInfo
DOX_MARKER([DOX_CONFIG_DEFINETYPE2])
namespace alib::config::detail {

void  VMeta_FormatMetaInfo::imPort(VDATA* data, Configuration&, const StringEscaper& escaper, const String& src)
{
    auto& rec      = data->As<alib::lox::textlogger::FormatMetaInfo>();
    LocalAllocator4K la;    
    StringVectorMA results(la);
    escaper.UnescapeTokens(results, src, A_CHAR(",") );

    rec.Format          .Reset(results.TryGet(0));
    rec.VerbosityError  .Reset(results.TryGet(1));
    rec.VerbosityWarning.Reset(results.TryGet(2));
    rec.VerbosityInfo   .Reset(results.TryGet(3));
    rec.VerbosityVerbose.Reset(results.TryGet(4));
    rec.MsgSuffix       .Reset(results.TryGet(5));
}

void  VMeta_FormatMetaInfo::exPort(VDATA* data, Configuration&, const StringEscaper& escaper, AString& dest)
{
    auto& rec= data->As<alib::lox::textlogger::FormatMetaInfo>();
    escaper.Escape(rec.Format           , dest, A_CHAR(",")); dest <<", " << NEW_LINE;
    escaper.Escape(rec.VerbosityError   , dest, A_CHAR(",")); dest <<", " << NEW_LINE;
    escaper.Escape(rec.VerbosityWarning , dest, A_CHAR(",")); dest <<", " << NEW_LINE;
    escaper.Escape(rec.VerbosityInfo    , dest, A_CHAR(",")); dest <<", " << NEW_LINE;
    escaper.Escape(rec.VerbosityVerbose , dest, A_CHAR(",")); dest <<", " << NEW_LINE;
    escaper.Escape(rec.MsgSuffix        , dest, A_CHAR(","));
}

} // namespace [alib::config::detail]
DOX_MARKER([DOX_CONFIG_DEFINETYPE2])

namespace alib::config::detail {

// FormatDate
ALIB_API void  VMeta_FormatDateTime::imPort(VDATA* data, Configuration&, const StringEscaper& escaper, const String& src)
{
    auto& rec= data->As<alib::lox::textlogger::FormatDateTime>();
    LocalAllocator4K la;
    StringVectorMA results(la);
    escaper.UnescapeTokens(results, src, A_CHAR(","));

    rec.Date       .Reset(results.TryGet(0));
    rec.TimeOfDay  .Reset(results.TryGet(1));
    rec.ElapsedDays.Reset(results.TryGet(2));
}
ALIB_API void  VMeta_FormatDateTime::exPort(VDATA* data, Configuration& cfg, const StringEscaper& escaper, AString& dest)
{
    (void)cfg;
    auto& rec= data->As<alib::lox::textlogger::FormatDateTime>();
    escaper.Escape(rec.Date         , dest, A_CHAR(",")); dest << A_CHAR(", ");
    escaper.Escape(rec.TimeOfDay    , dest, A_CHAR(",")); dest << A_CHAR(", ");
    escaper.Escape(rec.ElapsedDays  , dest, A_CHAR(","));
}

// FormatTimeDiff
ALIB_API void  VMeta_FormatTimeDiff::imPort(VDATA* data, Configuration&, const StringEscaper& escaper, const String& src)
{
    auto& rec= data->As<alib::lox::textlogger::FormatTimeDiff>();
    LocalAllocator4K la;
    StringVectorMA results(la);
    escaper.UnescapeTokens(results, src, A_CHAR(","));

    rec.Minimum  =int64_t(Substring( results.TryGet(0)).ParseDec());
    rec.None                 .Reset( results.TryGet(1));
    rec.Nanos                .Reset( results.TryGet(2));
    rec.Micros               .Reset( results.TryGet(3));
    rec.Millis               .Reset( results.TryGet(4));
    rec.Secs                 .Reset( results.TryGet(5));
    rec.Mins                 .Reset( results.TryGet(6));
    rec.Hours                .Reset( results.TryGet(7));
    rec.Days                 .Reset( results.TryGet(8));
}
ALIB_API void  VMeta_FormatTimeDiff::exPort(VDATA* data, Configuration& cfg, const StringEscaper& escaper, AString& dest)
{
    (void)cfg;
    auto& rec= data->As<alib::lox::textlogger::FormatTimeDiff>();
                                  dest <<   rec.Minimum; dest << A_CHAR(", ");
    escaper.Escape(rec.None     , dest, A_CHAR(","));    dest << A_CHAR(", ");
    escaper.Escape(rec.Nanos    , dest, A_CHAR(","));    dest << A_CHAR(", ");
    escaper.Escape(rec.Micros   , dest, A_CHAR(","));    dest << A_CHAR(", ");
    escaper.Escape(rec.Millis   , dest, A_CHAR(","));    dest << A_CHAR(", ");
    escaper.Escape(rec.Secs     , dest, A_CHAR(","));    dest << A_CHAR(", ");
    escaper.Escape(rec.Mins     , dest, A_CHAR(","));    dest << A_CHAR(", ");
    escaper.Escape(rec.Hours    , dest, A_CHAR(","));    dest << A_CHAR(", ");
    escaper.Escape(rec.Days     , dest, A_CHAR(","));
}

// FormatMultiLine
ALIB_API void  VMeta_FormatMultiLine::imPort(VDATA* data, Configuration&, const StringEscaper& escaper, const String& src)
{
    auto& rec= data->As<alib::lox::textlogger::FormatMultiLine>();
    LocalAllocator4K la;
    StringVectorMA results(la);
    escaper.UnescapeTokens(results, src, A_CHAR(","));
    String256 buf;

    rec.Mode  =int(Substring( results.TryGet(0) ).ParseDec());
    rec.Headline       .Reset(results.TryGet(1));
    rec.Prefix         .Reset(results.TryGet(2));
    rec.Suffix         .Reset(results.TryGet(3));
             buf       .Reset(results.TryGet(4));
    if( !buf.StartsWith<CHK, lang::Case::Ignore>(A_CHAR("null")) )
        rec.Delimiter  .Reset(buf);
    rec.DelimiterReplacement  .Reset(results.TryGet(5));
}

ALIB_API void  VMeta_FormatMultiLine::exPort(VDATA* data, Configuration& cfg, const StringEscaper& escaper, AString& dest)
{
    (void)cfg;
    auto& rec= data->As<alib::lox::textlogger::FormatMultiLine>();
         dest <<   rec.Mode                              ;   dest << A_CHAR(", ");
    escaper.Escape(rec.Headline      , dest, A_CHAR(","));   dest << A_CHAR(", ");
    escaper.Escape(rec.Prefix        , dest, A_CHAR(","));   dest << A_CHAR(", ");
    escaper.Escape(rec.Suffix        , dest, A_CHAR(","));   dest << A_CHAR(", ");
  if( rec.Delimiter.IsNotNull() )
    escaper.Escape(rec.Delimiter     , dest, A_CHAR(",")) << dest << A_CHAR(", ");
  else
    dest   << "nulled, ";
    escaper.Escape(rec.DelimiterReplacement , dest, A_CHAR(","));
}

// FormatOther
ALIB_API void  VMeta_FormatOther::imPort(VDATA* data, Configuration&, const StringEscaper& escaper, const String& src)
{
    auto& rec= data->As<alib::lox::textlogger::FormatOther>();
    LocalAllocator4K la;
    StringVectorMA results(la);
    escaper.UnescapeTokens(results, src, A_CHAR(","));

    rec.NoSourceFileInfo            .Reset(results.TryGet(0));
    rec.NoMethodInfo                .Reset(results.TryGet(1));
    rec.LogNumberMinDigits= int(Substring( results.TryGet(2) ).ParseDec());
}
ALIB_API void  VMeta_FormatOther::exPort(VDATA* data, Configuration&, const StringEscaper& escaper, AString& dest)
{
    auto& rec= data->As<alib::lox::textlogger::FormatOther>();
#if ALIB_CHARACTERS_WIDE
    String256 wNoSourceFileInfo(rec.NoSourceFileInfo);
    String256 wNoMethodInfo    (rec.NoMethodInfo    );
    escaper.Escape(wNoSourceFileInfo  , dest, A_CHAR(",")); dest << A_CHAR(", ");
    escaper.Escape(wNoMethodInfo      , dest, A_CHAR(",")); dest << A_CHAR(", ");
#else
    escaper.Escape(rec.NoSourceFileInfo  , dest, ","); dest << A_CHAR(", ");
    escaper.Escape(rec.NoMethodInfo      , dest, ","); dest << A_CHAR(", ");
#endif
           dest << rec.LogNumberMinDigits;
}

// FormatAutoSizes
ALIB_API void  VMeta_FormatAutoSizes::imPort(VDATA* data, Configuration&, const StringEscaper& , const String& src)
{
    auto& rec= data->As<alib::lox::textlogger::FormatAutoSizes>();

    Substring importMI(src);
    Substring importLog;
    integer sepPos= importMI.IndexOf(';');
    if( sepPos >= 0 )
        importMI.Split( sepPos, importLog, 1 );
    rec.Main      .Import( importMI );
    rec.LogMessage.Import( importLog );
}

ALIB_API void  VMeta_FormatAutoSizes::exPort(VDATA* data, Configuration&, const StringEscaper& , AString& dest)
{
    auto& rec= data->As<alib::lox::textlogger::FormatAutoSizes>();
    rec.Main      .Export( dest );
    dest << " ; ";
    rec.LogMessage.Export( dest );
}
    
// Replacements
ALIB_API void  VMeta_Replacements::imPort(VDATA* data, Configuration&, const StringEscaper& escaper, const String& src)
{
    auto& pairs= data->As<alib::lox::textlogger::Replacements>().Pairs;
    LocalAllocator4K la;
    StringVectorMA results(la);
    escaper.UnescapeTokens(results, src, A_CHAR("=,"));
    int cnt= 0;
    while(true)
    {
        String searchString=  results.TryGet(cnt );
        String replaceString= results.TryGet(cnt + 1);
        if( searchString.IsEmpty() )
            break;

        pairs.insert( pairs.end(), AStringPA(pairs.get_allocator().GetAllocator()) );
        pairs.back() << searchString;
        pairs.insert( pairs.end(), AStringPA(pairs.get_allocator().GetAllocator()) );
        pairs.back() << replaceString;
        cnt+= 2;
    }
}

ALIB_API void  VMeta_Replacements::exPort(VDATA* data, Configuration&, const StringEscaper& escaper, AString& dest)
{
    auto& pairs= data->As<alib::lox::textlogger::Replacements>().Pairs;
    for(auto it = pairs.begin() ; it != pairs.end() ; )
    {
        escaper.Escape(*it      , dest, A_CHAR("=,"));
        dest << "=";
        ++it;
        escaper.Escape(*it      , dest, A_CHAR("=,"));
        dest << ", ";
        ++it;
    }
    dest.DeleteEnd(A_CHAR(", "));
}

// ColorfulLoggerParameters
ALIB_API void  VMeta_ColorfulLoggerParameters::imPort(VDATA* data, Configuration&, const StringEscaper&, const String& src)
{
    auto& CLP= data->As<alib::lox::textlogger::ColorfulLoggerParameters>();
    Substring p= src;
    p.Trim();
    if( p.IsNotEmpty() && !enums::Parse<lox::textlogger::ColorfulLoggerParameters::LightColorUsage>( p, CLP.LCU ) )
    {
        ALIB_WARNING( "ALOX", "Error parsing variable CONSOLE_LIGHT_COLORS value {!Q'}.", src )
    }
}

ALIB_API void  VMeta_ColorfulLoggerParameters::exPort(VDATA* data, Configuration&, const StringEscaper&, AString& dest)
{
    auto& CLP= data->As<alib::lox::textlogger::ColorfulLoggerParameters>();
    dest << CLP.LCU;
}

} //namespace [alib::config::detail]
#endif //if !DOXYGEN


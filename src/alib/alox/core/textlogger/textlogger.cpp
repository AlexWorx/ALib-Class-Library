// #################################################################################################
//  aworx::lib::lox::core - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"
#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
    #include "alib/strings/util/tokenizer.hpp"
#endif
#if !defined (HPP_ALIB_SYSTEM_DIRECTORY)
    #include "alib/system/directory.hpp"
#endif
#if !defined (HPP_ALIB_SYSTEM_PROCESSINFO)
    #include "alib/system/process.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_FORMAT_FORMATTER_PYTHONSTYLE)
    #include "alib/strings/format/formatterpythonstyle.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_FORMAT_FORMATTER_JAVASTYLE)
    #include "alib/strings/format/formatterjavastyle.hpp"
#endif
#if !defined (HPP_ALIB_TIME_CALENDAR)
    #include "alib/time/calendar.hpp"
#endif
#if !defined (HPP_ALOX_CORE_TEXTLOGGER_TEXTLOGGER)
    #include "textlogger.hpp"
#endif
#if !defined (HPP_ALIB_COMPATIBILITY_STD_STRING)
    #include "alib/compatibility/std_string.hpp"
#endif
#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif
#if !defined (_GLIBCXX_CSTRING) && !defined(_CSTRING_)
    #include <cstring>
#endif

using namespace std;

// For code compatibility with ALox Java/C++
// We have to use underscore as the start of the name and for this have to disable a compiler
// warning. But this is a local code (cpp file) anyhow.
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

    #define _NC template _<false>

#if defined(__clang__)
    #pragma clang diagnostic pop
#endif


namespace aworx { namespace lib { namespace lox { namespace core { namespace textlogger {

// #################################################################################################
// StandardConverter
// #################################################################################################
StandardConverter::StandardConverter()
{
    FormatterPS.Next= &FormatterJS;
    cntRecursion= 0;
}

StandardConverter::~StandardConverter()
{
    ALIB_ASSERT_ERROR( cntRecursion == 0,  ASTR("Objectconverter recursion counter > 0.\n"
      "  Note: This error indicates, that a previous format operation (log statement) containted\n"
      "        corrupt format values, which caused the formatter to behave undefined, including\n"
      "        the corruption of the the execution stack."                                          ))
    for( auto* elem : recursionFormatters )
    {
        delete elem->Next;
        delete elem;
    }
}

void StandardConverter::ConvertObjects( AString& target, Boxes& logables  )
{
    cntRecursion++;
    ALIB_ASSERT_WARNING( cntRecursion < 5, ASTR("Logging recursion depth >= 5") );

    // get a formatter. We use a clone per recursion depth!
    FormatterPythonStyle* formatter;
    if( cntRecursion == 1 )
        formatter= &FormatterPS;
    else
    {
        // did we have this depth already? If not, create a new set of formatters formatter
        size_t recFormatNo= static_cast<size_t>( cntRecursion - 1 );
        if( recursionFormatters.size() <= recFormatNo )
        {
            formatter= new FormatterPythonStyle();
            formatter->Next= new FormatterJavaStyle();
            recursionFormatters.emplace_back( formatter );
        }
        else
            formatter= recursionFormatters[recFormatNo];

        // clone the settings from default formatter set
        formatter->CloneSettings( FormatterPS );
    }

    formatter->Format( target, logables );

    cntRecursion--;
}

// #################################################################################################
// MetaInfo
// #################################################################################################
int MetaInfo::Write( TextLogger& logger, AString& buf, core::Domain& domain, Verbosity verbosity, ScopeInfo& scope )
{
    int qtyTabStops= 0;

    // check
    if ( Format.IsEmpty() )
        return 0;

    // clear DateTime singleton
    callerDateTime.Year= (numeric_limits<int>::min)();

    Substring format( Format );
    for(;;)
    {
        // get next and log substring between commands
        integer idx= format.IndexOf( '%' );
        if ( idx >= 0 )
        {
            format.ConsumeChars<false, CurrentData::Keep>( idx, buf, 1 );
            qtyTabStops+= processVariable( logger, domain, verbosity, scope, buf, format );
        }
        else
        {
            buf._NC( format );
            break;
        }
    }
    return qtyTabStops;
}

int MetaInfo::processVariable( TextLogger&        logger,
                               core::Domain&      domain,
                               Verbosity          verbosity,
                               ScopeInfo&         scope,
                               AString&           dest,
                               Substring&         variable       )

{
    // process commands
    character c2;
    switch ( variable.ConsumeChar() )
    {
        // scope info
        case 'S':
        {
            // read sub command
            NString val;
            switch( c2= variable.ConsumeChar() )
            {
                case 'P':   // SP: full path
                {
                    val= scope.GetFullPath();
                    if ( val.IsEmpty() )
                        val= NoSourceFileInfo;
                } break;

                case 'p':   // Sp: trimmed path
                {
                    integer previousLength= dest.Length();
                    scope.GetTrimmedPath( dest );
                    if( dest.Length() != previousLength )
                        return 0;
                    val= NoSourceFileInfo;
                } break;

                case 'F':   // file name
                {
                    val= scope.GetFileName();
                    if ( val.IsEmpty() )
                        val= NoSourceFileInfo;
                } break;

                case 'f':   // file name without extension
                {
                    val= scope.GetFileNameWithoutExtension();
                    if ( val.IsEmpty() )
                        val= NoSourceFileInfo;
                } break;


                case 'M':   // method name
                {
                    val= scope.GetMethod();
                    if ( val.IsEmpty() )
                        val= NoMethodInfo;
                } break;

                case 'L':  // line number
                {
                    dest._NC( scope.GetLineNumber() );
                    return 0;
                }

                default:
                {
                    ALIB_WARN_ONCE( *this, FormatWarning,
                                    ASTR("Unknown format variable '%S{}' (only one warning)"), c2  );
                    val= "%ERROR";
                } break;
            }
            dest._( val );
            return 0;
        }

        // %Tx: Time
        case 'T':
        {
            c2= variable.ConsumeChar();

            // %TD: Date
            if ( c2 == 'D' )
            {
                // get time stamp as CalendarDateTime once
                if ( callerDateTime.Year == (numeric_limits<int>::min)() )
                    callerDateTime.Set( DateConverter.ToDateTime( scope.GetTimeStamp() ) );

                // if standard format, just write it out
                if ( DateFormat.Equals( ASTR("yyyy-MM-dd") ) )
                {
                    dest._NC( aworx::Format( callerDateTime.Year,     4 ) )._NC( '-' )
                        ._NC( aworx::Format( callerDateTime.Month,    2 ) )._NC( '-' )
                        ._NC( aworx::Format( callerDateTime.Day,      2 ) );
                }
                // user defined format
                else
                    callerDateTime.Format( DateFormat, dest  );

                return 0;
            }


            // %TT: Time of Day
            if ( c2 == 'T' )
            {
                // get time stamp as CalendarDateTime once
                if ( callerDateTime.Year == (numeric_limits<int>::min)() )
                    callerDateTime.Set( DateConverter.ToDateTime( scope.GetTimeStamp() ) );

                // avoid the allocation of a) a StringBuilder (yes, a string builder is allocated inside StringBuilder.AppendFormat!)
                // and b) a DateTime object, if the format is the unchanged standard. And it is faster anyhow.
                if ( TimeOfDayFormat.Equals( ASTR("HH:mm:ss") ) )
                {
                    dest._NC( aworx::Format(callerDateTime.Hour,    2) )._NC( ':' )
                        ._NC( aworx::Format(callerDateTime.Minute,  2) )._NC( ':' )
                        ._NC( aworx::Format(callerDateTime.Second,  2) );
                }

                // user defined format
                else
                    callerDateTime.Format( TimeOfDayFormat, dest  );
            }

            // %TC: Time elapsed since created
            else if ( c2 == 'C' )
            {
                auto elapsedTime= scope.GetTimeStamp() - logger.TimeOfCreation;

                if( MaxElapsedTime < elapsedTime )
                    MaxElapsedTime= elapsedTime;

                integer       maxElapsedSecs= MaxElapsedTime.InAbsoluteSeconds();
                CalendarDuration  elapsed( elapsedTime );

                if ( maxElapsedSecs >= 60*60*24 )  dest._NC( elapsed.Days  )._NC( TimeElapsedDays );
                if ( maxElapsedSecs >= 60*60    )  dest._NC( aworx::Format(elapsed.Hours  ,  maxElapsedSecs >= 60*60*10 ?  2 : 1 ) )._NC( ':' );
                if ( maxElapsedSecs >= 60       )  dest._NC( aworx::Format(elapsed.Minutes,  maxElapsedSecs >= 10*60    ?  2 : 1 ) )._NC( ':' );
                dest._NC( aworx::Format(elapsed.Seconds,  maxElapsedSecs > 9 ? 2 : 1)          )._NC( '.' );
                dest._NC( aworx::Format(elapsed.Milliseconds,  3) );
            }

            // %TL: Time elapsed since last log call
            else if ( c2 == 'L' )
                writeTimeDiff( dest, scope.GetTimeStamp().Since( logger.TimeOfLastLog ).InNanoseconds() );

            else
            {
                ALIB_WARN_ONCE( *this, FormatWarning,
                                ASTR("Unknown format variable '%T{}' (only one warning)"), c2 );
            }
            return 0;
        }


        // Thread
        case 't':
        {
            c2= variable.ConsumeChar();

            if ( c2 == 'N' )        // %tN: thread name
            {
                const String& threadName= scope.GetThreadNameAndID(nullptr);
                dest._NC( Format::Field( threadName, logger.AutoSizes.Next( threadName.Length(), 0 ), Alignment::Center ) );
            }
            else if ( c2 == 'I' )   // %tI: thread ID
            {
                String32 threadID; threadID._( scope.GetThreadID() );
                dest._NC( Format::Field( threadID,   logger.AutoSizes.Next( threadID  .Length(), 0 ), Alignment::Center ) );
            }
            else
            {
                ALIB_WARN_ONCE( *this, FormatWarning,
                                ASTR("Unknown format variable '%t{}' (only one warning)"), c2 );
            }
            return 0;
        }

        case 'L':
        {
            c2= variable.ConsumeChar();
                 if ( c2 == 'G' )     dest._NC( logger.GetName() );
            else if ( c2 == 'X' )     dest._NC( scope.GetLoxName() );
            else
            {
                ALIB_WARN_ONCE( *this, FormatWarning,
                                ASTR("Unknown format variable '%L{}' (only one warning)"), c2 );
            }
            return 0;
        }

        case 'P':
        {
            dest._NC( ProcessInfo::Current().Name );
            return 0;
        }

        case 'V':
            dest._NC(  verbosity == Verbosity::Error     ? VerbosityError
                     : verbosity == Verbosity::Warning   ? VerbosityWarning
                     : verbosity == Verbosity::Info      ? VerbosityInfo
                     :                                     VerbosityVerbose    );
            return 0;

        case 'D':
        {
            dest._( Format::Field( domain.FullPath, logger.AutoSizes.Next( domain.FullPath.Length(), 0 ), Alignment::Left ) );
            return 0;
        }

        case '#':

            dest._NC( aworx::Format( logger.CntLogs, LogNumberMinDigits ) );
            return 0;

        // A: Auto tab
        case 'A':
        {
            // read extra space from format string
            integer idx= 0;
            int extraSpace= static_cast<int>( NumberFormat::ParseDecDigits( variable, idx ) );
            if (idx == 0 )
                extraSpace=   1;
            else
                variable.ConsumeChars( idx );

            // insert ESC code to jump to next tab level
            extraSpace= min( extraSpace, 10 + ('Z'-'A') );
            char escseq[4]= {'\x1B', 't' }; // 3 + one (unused termination character)
            escseq[2]= extraSpace < 10 ?   ('0' + static_cast<char>(extraSpace) )
                                       :   ('A' + static_cast<char>(extraSpace) );

            dest._NC( escseq );
            return 1;
        }

        case 'N':
            dest._NC( logger.GetName() );
            return 0;

        default:
            ALIB_WARN_ONCE( *this, FormatWarning,
                            ASTR("Unknown format character '{}' (only one warning)"),
                            *( variable.Buffer() -1 ) );
            return 0;
   }// switch
}

void MetaInfo::writeTimeDiff( AString& buf, int64_t diffNanos )
{
    // unmeasurable?
    if ( diffNanos < TimeDiffMinimum )
    {
        buf._NC( TimeDiffNone );
        return;
    }

    if ( diffNanos < 1000 )
    {
        buf._NC( aworx::Format( diffNanos, 3 ) )._NC( TimeDiffNanos );
        return;
    }

    // we continue with micros
    int64_t diffMicros= diffNanos / 1000L;

    // below 1000 microseconds?
    if ( diffMicros < 1000 )
    {
        buf._NC( aworx::Format( diffMicros, 3 ) );
        buf._NC( TimeDiffMicros );
        return;
    }

    // below 1000 ms?
    if ( diffMicros < 1000000 )
    {
        buf._NC( aworx::Format( (diffMicros / 1000), 3 ) )._NC( TimeDiffMillis );
        return;
    }


    // below 10 secs (rounded) ?
    if ( diffMicros < 9995000 )
    {
        // convert to hundredth of secs
        int64_t hundredthSecs=  ((diffMicros / 1000) + 5) / 10;

        // print two digits after dot x.xx
        buf._NC( aworx::Format( (hundredthSecs / 100), 1 ) )
           ._NC( '.' )
           ._NC( aworx::Format( (hundredthSecs % 100), 2 ) )
           ._NC( TimeDiffSecs );
        return;
    }

    // convert to tenth of secs
    int64_t tenthSecs=  ((diffMicros / 10000) + 5) / 10 ;

    // below 100 secs ?
    if ( tenthSecs < 1000 )
    {
        // print one digits after dot xx.x (round value by adding 5 hundredth)
        buf._NC( aworx::Format( ( tenthSecs / 10 ), 2 ) )
           ._NC( '.' )
           ._NC( aworx::Format( ( tenthSecs % 10 ), 1 ) )
           ._NC( TimeDiffSecs );
        return;
    }

    //     below 10 mins ?
    if ( tenthSecs < 6000 )
    {
        // convert to hundredth of minutes
        int64_t hundredthMins=  tenthSecs / 6;

        // print two digits after dot x.xx
        buf._NC( aworx::Format( (hundredthMins / 100), 1 ) )
           ._NC( '.' )
           ._NC( aworx::Format( (hundredthMins % 100), 2 ) )
           ._NC( TimeDiffMins );
        return;
    }

    // convert to tenth of minutes
    int64_t tenthMins=  tenthSecs / 60;

    // below 100 mins ?
    if ( tenthMins < 1000 )
    {
        // print one digits after dot xx.x (round value by adding 5 hundredth)
        buf._NC( aworx::Format( (tenthMins / 10), 2 ) )
           ._NC( '.' )
           ._NC( aworx::Format( (tenthMins % 10), 1 ) )
           ._NC( TimeDiffMins );
        return;
    }

    // below ten hours?
    if ( tenthMins < 6000 )
    {
        // convert to hundredth of hours
        int64_t hundredthHours=  tenthMins / 6;

        // print two digits after dot x.xx
        buf._NC( aworx::Format( (hundredthHours / 100), 1 ) )
           ._NC( '.' )
           ._NC( aworx::Format( (hundredthHours % 100), 2 ))
           ._NC( TimeDiffHours );
        return;
    }

    // convert to tenth of minutes
    int64_t tenthHours=  tenthMins / 60;

    // below 10 hours ?
    if ( tenthHours < 1000 )
    {
        // print two digits after dot x.xx
        buf._NC( aworx::Format( (tenthHours / 10), 2 ) )
           ._NC( '.' )
           ._NC( aworx::Format( (tenthHours % 10), 1 ) )
           ._NC( TimeDiffHours );
        return;
    }

    // below 100 hours ?
    if ( tenthHours < 1000 )
    {
        // print one digits after dot xx.x (round value by adding 5 hundredth)
        buf._NC( aworx::Format( (tenthHours / 10), 2 ) )
           ._NC( '.' )
           ._NC( aworx::Format( ((tenthHours / 10) % 10), 1 ) )
           ._NC( TimeDiffHours );
        return;
    }

    // convert to hundredth of days
    int64_t hundredthDays=  tenthHours * 10 / 24;

    // below 10 days ?
    if ( hundredthDays < 1000 )
    {
        // print two digits after dot x.xx
        buf._NC( aworx::Format( (hundredthDays / 100), 1 ) )
           ._NC( '.' )
           ._NC( aworx::Format( (hundredthDays % 100), 2 ) )
           ._NC( TimeDiffDays );
        return;
    }

    // 10 days or more (print days plus one digit after the comma)
    // print one digits after dot xx.x (round value by adding 5 hundredth)
    buf ._NC( aworx::Format( (hundredthDays / 100), 2 ) )
        ._NC( '.' )
        ._NC( aworx::Format( ((hundredthDays / 10) % 10), 1 ) )
        ._NC( TimeDiffDays );
}


// #################################################################################################
// TextLogger
// #################################################################################################

TextLogger::TextLogger( const NString& pName, const NString& typeName, bool  pUsesStdStreams  )
: Logger( pName, typeName )
, logBuf( 256 )
, msgBuf( 256 )
, usesStdStreams( pUsesStdStreams )
{
    MetaInfo=    new textlogger::MetaInfo();
}

TextLogger::~TextLogger()
{
    delete MetaInfo;
    if (Converter)
        delete Converter;
    ALIB_ASSERT( msgBuf.IsEmpty() )
}

int   TextLogger::AddAcquirer( ThreadLock* newAcquirer )
{
    // register with ALIB lockers (if not done yet)
    if ( usesStdStreams )
    {
        int registrationCounter;
        {
            ALIB_LOCK_WITH( lock )
            registrationCounter= this->stdStreamLockRegistrationCounter++;
        }
        if ( registrationCounter == 0 )
             lib::ALIB.StdOutputStreamsLock.AddAcquirer( this );
    }

    // Variable AUTO_SIZES: use last sessions' values
    Variable variable( Variables::AUTO_SIZES   , GetName() );
    if ( ALOX.Config->Load( variable ) != Priorities::NONE )
        AutoSizes.Import( Substring(variable.GetString()) );

    // Variable MAX_ELAPSED_TIME: use last sessions' values
    variable.Declare( Variables::MAX_ELAPSED_TIME, GetName());
    if ( ALOX.Config->Load( variable ) != Priorities::NONE )
    {
        int maxInSecs= static_cast<int>( variable.GetInteger() );
        Substring attrValue;
        if ( variable.GetAttribute( ASTR("limit"), attrValue ) )
        {
            integer maxMax;
            attrValue.ConsumeInt( maxMax );
            if ( maxInSecs > maxMax )
                maxInSecs= static_cast<int>( maxMax );
        }
        MetaInfo->MaxElapsedTime= Ticks::Duration::FromSeconds( maxInSecs );
    }


    // Variable  <name>_FORMAT / <typeName>_FORMAT:
    VariableDecl variableDecl( Variables::FORMAT );
    ALIB_ASSERT_WARNING( variableDecl.DefaultValue().IsNull(),
                         ASTR("Default value of variable FORMAT should be kept null") );

    if(    ALOX.Config->Load( variable.Declare( variableDecl, GetName()     ) ) == Priorities::NONE
        && ALOX.Config->Load( variable.Declare( variableDecl, GetTypeName() ) ) == Priorities::NONE )
    {
        // no variable created, yet. Let's create a 'personal' one on our name
        variable.Declare( Variables::FORMAT, GetName() );
        variable.Add( MetaInfo->Format            );
        variable.Add( MetaInfo->VerbosityError    );
        variable.Add( MetaInfo->VerbosityWarning  );
        variable.Add( MetaInfo->VerbosityInfo     );
        variable.Add( MetaInfo->VerbosityVerbose  );
        variable.Add( FmtMsgSuffix                );
        ALOX.Config->Store( variable );
    }
    else
    {
                                   MetaInfo->Format          ._()._( variable.GetString(0) );
        if( variable.Size() >= 2 ) MetaInfo->VerbosityError  ._()._( variable.GetString(1) );
        if( variable.Size() >= 3 ) MetaInfo->VerbosityWarning._()._( variable.GetString(2) );
        if( variable.Size() >= 4 ) MetaInfo->VerbosityInfo   ._()._( variable.GetString(3) );
        if( variable.Size() >= 5 ) MetaInfo->VerbosityVerbose._()._( variable.GetString(4) );
        if( variable.Size() >= 6 ) FmtMsgSuffix              ._()._( variable.GetString(5) );
    }

    // Variable  <name>_FORMAT_DATE_TIME / <typeName>_FORMAT_DATE_TIME:
    variableDecl= VariableDecl( Variables::FORMAT_DATE_TIME );
    ALIB_ASSERT_WARNING( variableDecl.DefaultValue().IsNull(),
                         ASTR("Default value of variable FORMAT_DATE_TIME should be kept null") );
    if(    ALOX.Config->Load( variable.Declare( variableDecl, GetName()     ) ) == Priorities::NONE
        && ALOX.Config->Load( variable.Declare( variableDecl, GetTypeName() ) ) == Priorities::NONE )
    {
        // no variable created, yet. Let's create a 'personal' one on our name
        variable.Declare( Variables::FORMAT_DATE_TIME, GetName() );
        variable.Add( MetaInfo->DateFormat        );
        variable.Add( MetaInfo->TimeOfDayFormat   );
        variable.Add( MetaInfo->TimeElapsedDays   );
        ALOX.Config->Store( variable );
    }
    else
    {
                                   MetaInfo->DateFormat      ._()._( variable.GetString(0) );
        if( variable.Size() >= 2 ) MetaInfo->TimeOfDayFormat ._()._( variable.GetString(1) );
        if( variable.Size() >= 3 ) MetaInfo->TimeElapsedDays ._()._( variable.GetString(2) );
    }

    // Variable  <name>FORMAT_TIME_DIFF / <typeName>FORMAT_TIME_DIFF:
    variableDecl= VariableDecl( Variables::FORMAT_TIME_DIFF );
    ALIB_ASSERT_WARNING( variableDecl.DefaultValue().IsNull(),
                         ASTR("Default value of variable FORMAT_TIME_DIFF should be kept null") );
    if(    ALOX.Config->Load( variable.Declare( variableDecl, GetName()     )) == Priorities::NONE
        && ALOX.Config->Load( variable.Declare( variableDecl, GetTypeName() )) == Priorities::NONE )
    {
        // no variable created, yet. Let's create a 'personal' one on our name
        variable.Declare( Variables::FORMAT_TIME_DIFF, GetName() );
        variable.Add( MetaInfo->TimeDiffMinimum);
        variable.Add( MetaInfo->TimeDiffNone   );
        variable.Add( MetaInfo->TimeDiffNanos  );
        variable.Add( MetaInfo->TimeDiffMicros );
        variable.Add( MetaInfo->TimeDiffMillis );
        variable.Add( MetaInfo->TimeDiffSecs   );
        variable.Add( MetaInfo->TimeDiffMins   );
        variable.Add( MetaInfo->TimeDiffHours  );
        variable.Add( MetaInfo->TimeDiffDays   );
        ALOX.Config->Store( variable );
    }
    else
    {
                                   MetaInfo->TimeDiffMinimum=       variable.GetInteger   (0)  ;
        if( variable.Size() >= 2 ) MetaInfo->TimeDiffNone   ._()._( variable.GetString(1) );
        if( variable.Size() >= 3 ) MetaInfo->TimeDiffNanos  ._()._( variable.GetString(2) );
        if( variable.Size() >= 4 ) MetaInfo->TimeDiffMicros ._()._( variable.GetString(3) );
        if( variable.Size() >= 5 ) MetaInfo->TimeDiffMillis ._()._( variable.GetString(4) );
        if( variable.Size() >= 6 ) MetaInfo->TimeDiffSecs   ._()._( variable.GetString(5) );
        if( variable.Size() >= 7 ) MetaInfo->TimeDiffMins   ._()._( variable.GetString(6) );
        if( variable.Size() >= 8 ) MetaInfo->TimeDiffHours  ._()._( variable.GetString(7) );
        if( variable.Size() >= 9 ) MetaInfo->TimeDiffDays   ._()._( variable.GetString(8) );
    }

    // Variable  <name>FORMAT_MULTILINE / <typeName>FORMAT_MULTILINE:
    variableDecl= VariableDecl( Variables::FORMAT_MULTILINE );
    ALIB_ASSERT_WARNING( variableDecl.DefaultValue().IsNull(),
                         ASTR("Default value of variable FORMAT_MULTILINE should be kept null") );
    if(    ALOX.Config->Load( variable.Declare( variableDecl, GetName()     )) == Priorities::NONE
        && ALOX.Config->Load( variable.Declare( variableDecl, GetTypeName() )) == Priorities::NONE )
    {
        // no variable created, yet. Let's create a 'personal' one on our name
        variable.Declare( Variables::FORMAT_MULTILINE, GetName() );
        variable.Add( MultiLineMsgMode );
        variable.Add( FmtMultiLineMsgHeadline   );
        variable.Add( FmtMultiLinePrefix  );
        variable.Add( FmtMultiLineSuffix );
        ALOX.Config->Store( variable );
    }
    else
    {
                                   MultiLineMsgMode= static_cast<int>( variable.GetInteger(0) )  ;
        if( variable.Size() >= 2 ) FmtMultiLineMsgHeadline._()._( variable.GetString(1) );
        if( variable.Size() >= 3 ) FmtMultiLinePrefix     ._()._( variable.GetString(2) );
        if( variable.Size() >= 4 ) FmtMultiLineSuffix     ._()._( variable.GetString(3) );
        if( variable.Size() >= 5 ) { if (variable.GetString(4)->Equals<Case::Ignore>( ASTR("nulled") ) )
                                        MultiLineDelimiter.SetNull();
                                     else
                                        MultiLineDelimiter._()._( variable.GetString(4) );
                                   }
        if( variable.Size() >= 6 ) MultiLineDelimiterRepl ._()._( variable.GetString(5) );
    }

    // Variable  <name>FORMAT_REPLACEMENTS / <typeName>FORMAT_REPLACEMENTS:
    variableDecl= VariableDecl( Variables::REPLACEMENTS );
    ALIB_ASSERT_WARNING( variableDecl.DefaultValue().IsNull(),
                         ASTR("Default value of variable REPLACEMENTS should be kept null") );
    if(    ALOX.Config->Load( variable.Declare( variableDecl, GetName()     )) == Priorities::NONE
        || ALOX.Config->Load( variable.Declare( variableDecl, GetTypeName() )) == Priorities::NONE )
    {
        for( int i= 0; i< variable.Size() / 2 ; i++ )
        {
            AString* searchString=  variable.GetString(i * 2);
            AString* replaceString= variable.GetString(i * 2 + 1);
            if( searchString != nullptr  && replaceString != nullptr )
                SetReplacement( *searchString, *replaceString );
        }
    }

    // call parents' implementation
    return Logger::AddAcquirer( newAcquirer );
}

int   TextLogger::RemoveAcquirer( ThreadLock* acquirer )
{
    // de-register with ALIB lockers (if not done yet)
    if ( usesStdStreams )
    {
        int registrationCounter;
        {
            ALIB_LOCK_WITH( lock )
            registrationCounter= --this->stdStreamLockRegistrationCounter;
        }

        if ( registrationCounter == 0 )
            lib::ALIB.StdOutputStreamsLock.RemoveAcquirer( this );
    }

    // export autosizes to configuration
    Variable variable( Variables::AUTO_SIZES, Name );
    AutoSizes.Export( variable.Add() );
    ALOX.Config->Store( variable );

    // export "max elapsed time" to configuration
    variable.Declare( Variables::MAX_ELAPSED_TIME, Name );
    AString* destVal=  ALOX.Config->Load( variable ) != Priorities::NONE  ?   variable.GetString()
                                                                           :  &variable.Add();

    destVal->_()._( MetaInfo->MaxElapsedTime.InAbsoluteSeconds() );

    ALOX.Config->Store( variable );

    // call parents' implementation
    return Logger::RemoveAcquirer( acquirer );
}


void TextLogger::SetReplacement( const String& searched, const String& replacement )
{
    // if exists, replace replacement
    for( auto it= replacements.begin(); it < replacements.end(); it+= 2)
        if ( it->Equals( searched ) )
        {
            if ( replacement.IsNotNull() )
            {
                it++;
                *it= replacement;
                return;
            }

            replacements.erase( it );
            replacements.erase( it );
            return;
        }

    // append at the end
    if ( replacement != nullptr )
    {
        replacements.insert( replacements.end(), AString(searched   ) );
        replacements.insert( replacements.end(), AString(replacement) );
    }
}

void TextLogger::ClearReplacements()
{
    replacements.clear();
}

void TextLogger::Log( Domain& domain, Verbosity verbosity, Boxes& logables, ScopeInfo& scope )
{
    // check
    if (!Converter)
        Converter=  new textlogger::StandardConverter();

    // we store the current msgBuf length and reset the buffer to this length when exiting.
    // This allows recursive calls! Recursion might happen with the evaluation of the
    // logables (in the next line).
    integer msgBufStartLength= msgBuf.Length();
    Converter->ConvertObjects( msgBuf, logables );

    // replace strings in message
    for ( size_t i= 0; i < replacements.size() ; i+= 2 )
        msgBuf.SearchAndReplace( replacements[i],
                                 replacements[i + 1], msgBufStartLength );

    // setup log buffer with meta info << ESC::EOMETA
    logBuf.Clear();
    AutoSizes.Start();
    int qtyESCTabsWritten= MetaInfo->Write( *this, logBuf, domain, verbosity, scope );
    logBuf._NC( ESC::EOMETA );

    // check for empty messages
    if ( msgBuf.Length() == msgBufStartLength )
    {
        // log empty msg and quit
        logBuf._NC( FmtMsgSuffix );
        if (usesStdStreams) lib::ALIB.StdOutputStreamsLock.Acquire(ALIB_CALLER_PRUNED);
            logText( domain, verbosity, logBuf, scope, -1 );
        if (usesStdStreams) lib::ALIB.StdOutputStreamsLock.Release();
        return;
    }

    // single line output
    if ( MultiLineMsgMode == 0 )
    {
        // replace line separators
        integer cntReplacements=0;
        if ( MultiLineDelimiter.IsNotNull() )
            cntReplacements+=    msgBuf.SearchAndReplace( MultiLineDelimiter, MultiLineDelimiterRepl, msgBufStartLength );
        else
        {
            String& replacement= MultiLineDelimiterRepl;
            cntReplacements+=    msgBuf.SearchAndReplace( ASTR("\r\n"), replacement, msgBufStartLength );
            cntReplacements+=    msgBuf.SearchAndReplace( ASTR("\r"),   replacement, msgBufStartLength );
            cntReplacements+=    msgBuf.SearchAndReplace( ASTR("\n"),   replacement, msgBufStartLength );
        }

        // append msg to logBuf
        if ( cntReplacements == 0 )
        {
            logBuf._NC( msgBuf, msgBufStartLength, msgBuf.Length() - msgBufStartLength );
        }
        else
        {
            logBuf._NC( FmtMultiLinePrefix );
            logBuf._NC( msgBuf, msgBufStartLength, msgBuf.Length() - msgBufStartLength );
            logBuf._NC( FmtMultiLineSuffix );
        }
        logBuf._NC( FmtMsgSuffix );

        // now do the logging by calling our derived classes' logText
        if (usesStdStreams) lib::ALIB.StdOutputStreamsLock.Acquire(ALIB_CALLER_PRUNED);
            logText( domain, verbosity, logBuf, scope, -1 );
        if (usesStdStreams) lib::ALIB.StdOutputStreamsLock.Release();

        // stop here
        msgBuf.SetLength( msgBufStartLength );
        return;
    }

    // multiple line output
    int      qtyTabStops= AutoSizes.ActualIndex;
    integer  actStart=    msgBufStartLength;
    int      lineNo=      0;
    integer  lbLenBeforeMsgPart= logBuf.Length();

    // loop over lines in msg
    while ( actStart < msgBuf.Length() )
    {
        // find next end
        integer delimLen;
        integer actEnd;

        // no delimiter given: search '\n' and then see if it is "\r\n" in fact
        if ( MultiLineDelimiter.IsEmpty() )
        {
            delimLen= 1;

            actEnd= msgBuf.IndexOf<false>( '\n', actStart );
            if( actEnd > actStart )
            {
                if( msgBuf.CharAt<false>(actEnd - 1) == '\r' )
                {
                    actEnd--;
                    delimLen= 2;
                }
            }
        }
        else
        {
            delimLen=  MultiLineDelimiter.Length();
            actEnd=    msgBuf.IndexOf<false>( MultiLineDelimiter, actStart );
        }

        // not found a delimiter? - log the rest
        if ( actEnd < 0 )
        {
            // single line?
            if ( lineNo == 0 )
            {
                logBuf._NC( msgBuf, msgBufStartLength, msgBuf.Length() - msgBufStartLength );
                logBuf._NC( FmtMsgSuffix );

                if (usesStdStreams) lib::ALIB.StdOutputStreamsLock.Acquire(ALIB_CALLER_PRUNED);
                    logText( domain, verbosity, logBuf, scope, -1 );
                if (usesStdStreams) lib::ALIB.StdOutputStreamsLock.Release();

                // stop here
                msgBuf.SetLength( msgBufStartLength );
                return;
            }

            // store actual end
            actEnd= msgBuf.Length();
        }

        // found a delimiter

        // signal start of multi line log
        if ( lineNo == 0 )
        {
            if (usesStdStreams) lib::ALIB.StdOutputStreamsLock.Acquire(ALIB_CALLER_PRUNED);
            notifyMultiLineOp( Phase::Begin );
        }

        // in mode 3, 4, meta info is deleted
        if ( lineNo == 0 && ( MultiLineMsgMode == 3 || MultiLineMsgMode == 4 ) )
        {
            // log headline in mode 3
            if ( MultiLineMsgMode == 3 )
            {
                logBuf._NC( FmtMultiLineMsgHeadline );
                AutoSizes.ActualIndex=  qtyTabStops;
                logText( domain, verbosity, logBuf, scope, 0 );
            }

            // remember zero as offset
            lbLenBeforeMsgPart=0;
        }

        // clear meta information?
        if ( MultiLineMsgMode == 2 )
        {
            if (lineNo != 0 )
            {
                logBuf.Clear()._( ESC::EOMETA );
                AutoSizes.ActualIndex=  qtyTabStops + qtyESCTabsWritten;
            }
        }
        // reset logBuf length to marked position
        else
        {
            logBuf.SetLength<false>( lbLenBeforeMsgPart );
            AutoSizes.ActualIndex=  qtyTabStops;
        }

        // append message and do the log
        logBuf._NC( FmtMultiLinePrefix );
          logBuf._NC( msgBuf,  actStart, actEnd - actStart  );
        logBuf._NC( FmtMultiLineSuffix );
        actStart= actEnd + delimLen;
        if ( actStart >= msgBuf.Length() )
            logBuf._NC( FmtMsgSuffix );
        logText( domain, verbosity, logBuf, scope, lineNo );

        // next
        lineNo++;
    }

    // signal end of multi line log
    if ( lineNo > 0 )
    {
        notifyMultiLineOp( Phase::End );
        if (usesStdStreams) lib::ALIB.StdOutputStreamsLock.Release();
    }

    msgBuf.SetLength( msgBufStartLength );
}

}}}}}// namespace [aworx::lib::lox::core::textlogger]


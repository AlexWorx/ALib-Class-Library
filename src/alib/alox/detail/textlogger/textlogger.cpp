// #################################################################################################
//  aworx::lib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALOX_DETAIL_TEXTLOGGER_TEXTLOGGER)
       #include "alib/alox/detail/textlogger/textlogger.hpp"
#   endif
#   define HPP_ALIB_LOX_PROPPERINCLUDE
#      if !defined (HPP_ALOX_DETAIL_DOMAIN)
#          include "alib/alox/detail/domain.inl"
#      endif
#      if !defined (HPP_ALOX_DETAIL_SCOPEINFO)
#          include "alib/alox/detail/scopeinfo.inl"
#      endif
#   undef HPP_ALIB_LOX_PROPPERINCLUDE

#   if !defined (HPP_ALIB_SYSTEM_PROCESSINFO)
       #include "alib/system/processinfo.hpp"
#   endif

#   if !defined (HPP_ALIB_ALOXMODULE)
#      include "alib/alox/aloxmodule.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_FORMAT)
#       include "alib/strings/format.hpp"
#   endif
#endif // !defined(ALIB_DOX)

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


namespace aworx { namespace lib { namespace lox { namespace detail { namespace textlogger {

// #################################################################################################
// StandardConverter
// #################################################################################################
StandardConverter::StandardConverter()
{
    auto* firstLevelFormatter= new FormatterPythonStyle();
    firstLevelFormatter->Next.reset( new FormatterJavaStyle()   );
    #if ALIB_THREADS
        firstLevelFormatter->SetSafeness( Safeness::Unsafe );
        firstLevelFormatter->Next->SetSafeness( Safeness::Unsafe );
        firstLevelFormatter->Acquire( ALIB_CALLER_PRUNED );
    #endif

    Formatters.emplace_back( firstLevelFormatter );
    cntRecursionx= -1;
}

StandardConverter::~StandardConverter()
{
    ALIB_ASSERT_ERROR( cntRecursionx == -1,
        "ALox object converter recursion counter > 0.\n"
        "Note: This error indicates, that a previous format operation (log statement) contained\n"
        "      corrupt format values, which caused the formatter to behave undefined, including\n"
        "      the corruption of the execution stack of ALox logging."                             )
    for( auto* elem : Formatters )
    {
        ALIB_IF_THREADS( elem->Release(); )
        delete elem;
    }
}

void StandardConverter::ConvertObjects( AString& target, Boxes& logables  )
{
    ++cntRecursionx;

        ALIB_ASSERT_WARNING( cntRecursionx < 5, "Logging recursion depth >= 5" )

        // get a formatter. We use a clone per recursion depth!
        // So, did we have this depth already? If not, create a new set of formatters formatter
        if( static_cast<size_t>( cntRecursionx ) >= Formatters.size()  )
        {
            // create a pair of recursion formatters
            Formatter* recursionFormatter= new FormatterPythonStyle();
            recursionFormatter->Next.reset( new FormatterJavaStyle() );

            #if ALIB_THREADS
                recursionFormatter      ->SetSafeness( Safeness::Unsafe );
                recursionFormatter->Next->SetSafeness( Safeness::Unsafe );
                recursionFormatter->Acquire( ALIB_CALLER_PRUNED );
            #endif

            recursionFormatter->CloneSettings( *Formatters[0] );

            Formatters.emplace_back( recursionFormatter );
        }

        Formatter* formatter= Formatters[static_cast<size_t>( cntRecursionx )];

        try
        {
            formatter->FormatArgs( target, logables );
        }
        catch(Exception& e )
        {
            target << ALOX.GetResource("TLFmtExc");
            e.Format( target );
        }

    --cntRecursionx;
}

AutoSizes* StandardConverter::GetAutoSizes()
{
    FormatterPythonStyle* fmtPS= dynamic_cast<FormatterPythonStyle*>( Formatters[0] );
    if (fmtPS != nullptr )
        return &fmtPS->Sizes;
    return nullptr;
}

void StandardConverter::ResetAutoSizes()
{
    FormatterPythonStyle* fmtPS;
    for( auto* elem : Formatters )
        if ( (fmtPS= dynamic_cast<FormatterPythonStyle*>( elem )) != nullptr )
            fmtPS->Sizes.Reset();
}

// #################################################################################################
// MetaInfo
// #################################################################################################
void MetaInfo::Write( TextLogger& logger, AString& buf, detail::Domain& domain, Verbosity verbosity, ScopeInfo& scope )
{
    // check
    if ( Format.IsEmpty() )
        return;

    // clear DateTime singleton
    callerDateTime.Year= (std::numeric_limits<int>::min)();

    Substring format( Format );
    for(;;)
    {
        // get next and log sub-string between commands
        integer idx= format.IndexOf( '%' );
        if ( idx >= 0 )
        {
            format.ConsumeChars<false, CurrentData::Keep>( idx, buf, 1 );
            processVariable( logger, domain, verbosity, scope, buf, format );
        }
        else
        {
            buf._NC( format );
            break;
        }
    }
}

void MetaInfo::processVariable( TextLogger&        logger,
                                detail::Domain&      domain,
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
                        return;
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
                    return;
                }

                default:
                {
                    ALIB_ASSERT_WARNING( FormatWarningOnce,
                                         "Unknown format variable '%S{}' (only one warning)", c2  )
                    ALIB_DBG( FormatWarningOnce= true; )
                    val= "%ERROR";
                } break;
            }
            dest._( val );
            return;
        }

        // %Tx: Time
        case 'T':
        {
            c2= variable.ConsumeChar();

            // %TD: Date
            if ( c2 == 'D' )
            {
                // get time stamp as CalendarDateTime once
                if ( callerDateTime.Year == (std::numeric_limits<int>::min)() )
                    callerDateTime.Set( DateConverter.ToDateTime( scope.GetTimeStamp() ) );

                // if standard format, just write it out
                if ( DateFormat.Equals( A_CHAR("yyyy-MM-dd") ) )
                {
                    dest._NC( aworx::Format( callerDateTime.Year,     4 ) )._NC( '-' )
                        ._NC( aworx::Format( callerDateTime.Month,    2 ) )._NC( '-' )
                        ._NC( aworx::Format( callerDateTime.Day,      2 ) );
                }
                // user defined format
                else
                    callerDateTime.Format( DateFormat, dest  );

                return;
            }


            // %TT: Time of Day
            if ( c2 == 'T' )
            {
                // get time stamp as CalendarDateTime once
                if ( callerDateTime.Year == (std::numeric_limits<int>::min)() )
                    callerDateTime.Set( DateConverter.ToDateTime( scope.GetTimeStamp() ) );

                // avoid the allocation of a) a StringBuilder (yes, a string builder is allocated inside StringBuilder.AppendFormat!)
                // and b) a DateTime object, if the format is the unchanged standard. And it is faster anyhow.
                if ( TimeOfDayFormat.Equals( A_CHAR("HH:mm:ss") ) )
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

                auto       maxElapsedSecs= MaxElapsedTime.InAbsoluteSeconds();
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
                ALIB_ASSERT_WARNING( FormatWarningOnce,
                                     "Unknown format variable '%T{}' (only one warning)", c2  )
                ALIB_DBG( FormatWarningOnce= true; )
            }
            return;
        }


        // Thread
        case 't':
        {
            c2= variable.ConsumeChar();

            if ( c2 == 'N' )        // %tN: thread name
            {
                #if ALIB_THREADS
                    const String& threadName= scope.GetThreadNameAndID(nullptr);
                #else
                    String msg( A_CHAR("SINGLE_THREADED") );
                    const String& threadName= msg;
                #endif
                dest._NC( Format::Field( threadName, logger.AutoSizes.Next( AutoSizes::Types::Field, threadName.Length(), 0 ), Alignment::Center ) );
            }
            else if ( c2 == 'I' )   // %tI: thread ID
            {
                String32 threadID;
                #if ALIB_THREADS
                    threadID._( scope.GetThreadID() );
                #else
                    threadID << "-1";
                #endif
                dest._NC( Format::Field( threadID,   logger.AutoSizes.Next( AutoSizes::Types::Field, threadID  .Length(), 0 ), Alignment::Center ) );
            }
            else
            {
                ALIB_ASSERT_WARNING( FormatWarningOnce,
                                     "Unknown format variable '%t{}' (only one warning)", c2  )
                ALIB_DBG( FormatWarningOnce= true; )
            }
            return;
        }

        case 'L':
        {
            c2= variable.ConsumeChar();
                 if ( c2 == 'G' )     dest._NC( logger.GetName() );
            else if ( c2 == 'X' )     dest._NC( scope.GetLoxName() );
            else
            {
                ALIB_ASSERT_WARNING( FormatWarningOnce,
                                     "Unknown format variable '%L{}' (only one warning)", c2  )
                ALIB_DBG( FormatWarningOnce= true; )
            }
            return;
        }

        case 'P':
        {
            dest._NC( ProcessInfo::Current().Name );
            return;
        }

        case 'V':
            dest._NC(  verbosity == Verbosity::Error     ? VerbosityError
                     : verbosity == Verbosity::Warning   ? VerbosityWarning
                     : verbosity == Verbosity::Info      ? VerbosityInfo
                     :                                     VerbosityVerbose    );
            return;

        case 'D':
        {
            dest._( Format::Field( domain.FullPath, logger.AutoSizes.Next( AutoSizes::Types::Field, domain.FullPath.Length(), 0 ), Alignment::Left ) );
            return;
        }

        case '#':
            dest._NC( aworx::Format( logger.CntLogs, LogNumberMinDigits ) );
            return;

        // A: Auto tab
        case 'A':
        {
            // read extra space from format string
            integer extraSpace;
            if( !variable.ConsumeDecDigits( extraSpace ) )
                extraSpace= 1;
            integer currentLength= dest.WStringLength();
            integer tabPos= logger.AutoSizes.Next( AutoSizes::Types::Tabstop, currentLength, extraSpace );
            dest.InsertChars(' ', tabPos - currentLength );

            return;
        }

        case 'N':
            dest._NC( logger.GetName() );
            return;

        default:
            ALIB_ASSERT_WARNING( FormatWarningOnce,
                        "Unknown format character {!Q'} (only one warning)",
                        *( variable.Buffer() -1 ) )
            ALIB_DBG( FormatWarningOnce= true; )
            return;
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
, usesStdStreams( pUsesStdStreams )
{
    logBuf.SetBuffer( 256 );
    msgBuf.SetBuffer( 256 );
    MetaInfo=    new textlogger::MetaInfo();
}

TextLogger::~TextLogger()
{
    delete MetaInfo;
    if (Converter)
        delete Converter;
    ALIB_ASSERT( msgBuf.IsEmpty() )
}

void   TextLogger::AcknowledgeLox( LoxImpl* lox, ContainerOp op )
{
    ALIB_IFN_THREADS( (void) lox; )

    Variable cfgVar;

    // ---------------  insert ------------------
    if( op == ContainerOp::Insert )
    {
        if ( Converter == nullptr )
            Converter= new textlogger::StandardConverter();

        // register with ALIB lockers (if not done yet)
        if ( usesStdStreams )
        {
            #if ALIB_THREADS
                int registrationCounter;
                {
                    ALIB_LOCK_WITH( lock )
                    registrationCounter= stdStreamLockRegistrationCounter++;
                }
                if ( registrationCounter == 0 )
                     SmartLock::StdOutputStreams.AddAcquirer( this );
            #endif
        }

        // Variable AUTO_SIZES: use last session's values
        cfgVar.Declare( Variables::AUTO_SIZES, GetName() );
        if ( ALOX.GetConfig().Load( cfgVar ) != Priorities::NONE )
        {
            Substring importMI= cfgVar.GetString();
            Substring importLog;
            integer sepPos= importMI.IndexOf(';');
            if( sepPos >= 0 )
                importMI.Split( sepPos, importLog, 1 );
            AutoSizes.Import( importMI );

            strings::util::AutoSizes* autoSizesLog= Converter->GetAutoSizes();
            if( autoSizesLog != nullptr && importLog.IsNotEmpty() )
            {
                autoSizesLog->Import( importLog );
            }
        }


        // Variable MAX_ELAPSED_TIME: use last session's values
        cfgVar.Declare( Variables::MAX_ELAPSED_TIME, GetName());
        if ( ALOX.GetConfig().Load( cfgVar ) != Priorities::NONE )
        {
            int maxInSecs= static_cast<int>( cfgVar.GetInteger() );
            Substring attrValue;
            if ( cfgVar.GetAttribute( A_CHAR( "limit"), attrValue ) )
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
        ALIB_ASSERT_WARNING( variableDecl.DefaultValue.IsNull(),
                             "Default value of variable FORMAT should be kept null" )

        if( ALOX.GetConfig().Load( cfgVar.Declare( variableDecl, GetName()     ) ) == Priorities::NONE
            && ALOX.GetConfig().Load( cfgVar.Declare( variableDecl, GetTypeName() ) ) == Priorities::NONE )
        {
            // no variable created, yet. Let's create a 'personal' one on our name
            cfgVar.Declare( Variables::FORMAT, GetName() );
            cfgVar.Add( MetaInfo->Format            );
            cfgVar.Add( MetaInfo->VerbosityError    );
            cfgVar.Add( MetaInfo->VerbosityWarning  );
            cfgVar.Add( MetaInfo->VerbosityInfo     );
            cfgVar.Add( MetaInfo->VerbosityVerbose  );
            cfgVar.Add( FmtMsgSuffix                );
            ALOX.GetConfig().Store( cfgVar );
        }
        else
        {
                                      MetaInfo->Format          .Reset( cfgVar.GetString( 0) );
            if( cfgVar.Size() >= 2 ) MetaInfo->VerbosityError  .Reset( cfgVar.GetString( 1) );
            if( cfgVar.Size() >= 3 ) MetaInfo->VerbosityWarning.Reset( cfgVar.GetString( 2) );
            if( cfgVar.Size() >= 4 ) MetaInfo->VerbosityInfo   .Reset( cfgVar.GetString( 3) );
            if( cfgVar.Size() >= 5 ) MetaInfo->VerbosityVerbose.Reset( cfgVar.GetString( 4) );
            if( cfgVar.Size() >= 6 ) FmtMsgSuffix              .Reset( cfgVar.GetString( 5) );
        }

        // Variable  <name>_FORMAT_DATE_TIME / <typeName>_FORMAT_DATE_TIME:
        variableDecl= VariableDecl( Variables::FORMAT_DATE_TIME );
        ALIB_ASSERT_WARNING( variableDecl.DefaultValue.IsNull(),
                             "Default value of variable FORMAT_DATE_TIME should be kept null" )
        if( ALOX.GetConfig().Load( cfgVar.Declare( variableDecl, GetName()     ) ) == Priorities::NONE
            && ALOX.GetConfig().Load( cfgVar.Declare( variableDecl, GetTypeName() ) ) == Priorities::NONE )
        {
            // no variable created, yet. Let's create a 'personal' one on our name
            cfgVar.Declare( Variables::FORMAT_DATE_TIME, GetName() );
            cfgVar.Add( MetaInfo->DateFormat        );
            cfgVar.Add( MetaInfo->TimeOfDayFormat   );
            cfgVar.Add( MetaInfo->TimeElapsedDays   );
            ALOX.GetConfig().Store( cfgVar );
        }
        else
        {
                                      MetaInfo->DateFormat      .Reset( cfgVar.GetString( 0) );
            if( cfgVar.Size() >= 2 ) MetaInfo->TimeOfDayFormat .Reset( cfgVar.GetString( 1) );
            if( cfgVar.Size() >= 3 ) MetaInfo->TimeElapsedDays .Reset( cfgVar.GetString( 2) );
        }

        // Variable  <name>FORMAT_TIME_DIFF / <typeName>FORMAT_TIME_DIFF:
        variableDecl= VariableDecl( Variables::FORMAT_TIME_DIFF );
        ALIB_ASSERT_WARNING( variableDecl.DefaultValue.IsNull(),
                             "Default value of variable FORMAT_TIME_DIFF should be kept null" )
        if( ALOX.GetConfig().Load( cfgVar.Declare( variableDecl, GetName()     )) == Priorities::NONE
            && ALOX.GetConfig().Load( cfgVar.Declare( variableDecl, GetTypeName() )) == Priorities::NONE )
        {
            // no variable created, yet. Let's create a 'personal' one on our name
            cfgVar.Declare( Variables::FORMAT_TIME_DIFF, GetName() );
            cfgVar.Add( MetaInfo->TimeDiffMinimum);
            cfgVar.Add( MetaInfo->TimeDiffNone   );
            cfgVar.Add( MetaInfo->TimeDiffNanos  );
            cfgVar.Add( MetaInfo->TimeDiffMicros );
            cfgVar.Add( MetaInfo->TimeDiffMillis );
            cfgVar.Add( MetaInfo->TimeDiffSecs   );
            cfgVar.Add( MetaInfo->TimeDiffMins   );
            cfgVar.Add( MetaInfo->TimeDiffHours  );
            cfgVar.Add( MetaInfo->TimeDiffDays   );
            ALOX.GetConfig().Store( cfgVar );
        }
        else
        {
                                      MetaInfo->TimeDiffMinimum=        cfgVar.GetInteger   ( 0)  ;
            if( cfgVar.Size() >= 2 ) MetaInfo->TimeDiffNone   .Reset( cfgVar.GetString( 1) );
            if( cfgVar.Size() >= 3 ) MetaInfo->TimeDiffNanos  .Reset( cfgVar.GetString( 2) );
            if( cfgVar.Size() >= 4 ) MetaInfo->TimeDiffMicros .Reset( cfgVar.GetString( 3) );
            if( cfgVar.Size() >= 5 ) MetaInfo->TimeDiffMillis .Reset( cfgVar.GetString( 4) );
            if( cfgVar.Size() >= 6 ) MetaInfo->TimeDiffSecs   .Reset( cfgVar.GetString( 5) );
            if( cfgVar.Size() >= 7 ) MetaInfo->TimeDiffMins   .Reset( cfgVar.GetString( 6) );
            if( cfgVar.Size() >= 8 ) MetaInfo->TimeDiffHours  .Reset( cfgVar.GetString( 7) );
            if( cfgVar.Size() >= 9 ) MetaInfo->TimeDiffDays   .Reset( cfgVar.GetString( 8) );
        }

        // Variable  <name>FORMAT_MULTILINE / <typeName>FORMAT_MULTILINE:
        variableDecl= VariableDecl( Variables::FORMAT_MULTILINE );
        ALIB_ASSERT_WARNING( variableDecl.DefaultValue.IsNull(),
                             "Default value of variable FORMAT_MULTILINE should be kept null" )
        if( ALOX.GetConfig().Load( cfgVar.Declare( variableDecl, GetName()     )) == Priorities::NONE
            && ALOX.GetConfig().Load( cfgVar.Declare( variableDecl, GetTypeName() )) == Priorities::NONE )
        {
            // no variable created, yet. Let's create a 'personal' one on our name
            cfgVar.Declare( Variables::FORMAT_MULTILINE, GetName() );
            cfgVar.Add( MultiLineMsgMode );
            cfgVar.Add( FmtMultiLineMsgHeadline   );
            cfgVar.Add( FmtMultiLinePrefix  );
            cfgVar.Add( FmtMultiLineSuffix );
            ALOX.GetConfig().Store( cfgVar );
        }
        else
        {
                                      MultiLineMsgMode= static_cast<int>( cfgVar.GetInteger( 0) )  ;
            if( cfgVar.Size() >= 2 ) FmtMultiLineMsgHeadline.Reset( cfgVar.GetString( 1) );
            if( cfgVar.Size() >= 3 ) FmtMultiLinePrefix     .Reset( cfgVar.GetString( 2) );
            if( cfgVar.Size() >= 4 ) FmtMultiLineSuffix     .Reset( cfgVar.GetString( 3) );
            if( cfgVar.Size() >= 5 ) { if (cfgVar.GetString( 4).Equals<Case::Ignore>( A_CHAR( "nulled") ) )
                                            MultiLineDelimiter.SetNull();
                                         else
                                            MultiLineDelimiter.Reset( cfgVar.GetString( 4) );
                                       }
            if( cfgVar.Size() >= 6 ) MultiLineDelimiterRepl .Reset( cfgVar.GetString( 5) );
        }

        // Variable  <name>FORMAT_REPLACEMENTS / <typeName>FORMAT_REPLACEMENTS:
        variableDecl= VariableDecl( Variables::REPLACEMENTS );
        ALIB_ASSERT_WARNING( variableDecl.DefaultValue.IsNull(),
                             "Default value of variable REPLACEMENTS should be kept null" )
        if( ALOX.GetConfig().Load( cfgVar.Declare( variableDecl, GetName()     )) == Priorities::NONE
            || ALOX.GetConfig().Load( cfgVar.Declare( variableDecl, GetTypeName() )) == Priorities::NONE )
        {
            for( int i= 0; i + 1 < cfgVar.Size() ; i+= 2 )
            {
                String searchString=  cfgVar.GetString( i    );
                String replaceString= cfgVar.GetString( i + 1);
                SetReplacement( searchString, replaceString );
            }
        }

        // register the lox' lock with us (being a smartlock)
        ALIB_IF_THREADS( SmartLock::AddAcquirer( &LI::getLock(lox) ); )

        return;
    }

    // ---------------  remove ------------------

    // de-register with ALIB lockers (if not done yet)
    if ( usesStdStreams )
    {
        #if ALIB_THREADS
            int registrationCounter;
            {
                ALIB_LOCK_WITH( lock )
                registrationCounter= --this->stdStreamLockRegistrationCounter;
            }

            if ( registrationCounter == 0 )
                SmartLock::StdOutputStreams.RemoveAcquirer( this );
        #endif
    }

    // export auto sizes to configuration
    cfgVar.Declare( Variables::AUTO_SIZES, Name );
    String256 exportString; exportString.DbgDisableBufferReplacementWarning();
    AutoSizes.Export( exportString );
    strings::util::AutoSizes* autoSizesLog= Converter->GetAutoSizes();
    if( autoSizesLog != nullptr )
    {
        exportString._( ';' );
        autoSizesLog->Export( exportString );
    }
    cfgVar.Add( exportString );
    ALOX.GetConfig().Store( cfgVar );


    // export "max elapsed time" to configuration
    String128 destVal( MetaInfo->MaxElapsedTime.InAbsoluteSeconds() );
    cfgVar.Declare( Variables::MAX_ELAPSED_TIME, Name );
    if( ALOX.GetConfig().Load( cfgVar ) != Priorities::NONE )
        cfgVar.ReplaceValue(0, destVal);
    else
        cfgVar.Add(destVal);

    ALOX.GetConfig().Store( cfgVar );

    // de-register the lox' lock from us (being a smartlock)
    ALIB_IF_THREADS( SmartLock::RemoveAcquirer( &LI::getLock(lox) ); )
}


void TextLogger::SetReplacement( const String& searched, const String& replacement )
{
    // if exists, replace replacement
    for( auto it= replacements.begin(); it < replacements.end(); it+= 2)
        if ( it->Equals( searched ) )
        {
            if ( replacement.IsNotNull() )
            {
                ++it;
                (*it).Reset( replacement );
                return;
            }

            replacements.erase( it );
            replacements.erase( it );
            return;
        }

    // append at the end
    if ( replacement.IsNotNull() )
    {
        replacements.insert( replacements.end(), AString(searched   ) );
        replacements.insert( replacements.end(), AString(replacement) );
    }
}

void TextLogger::ClearReplacements()
{
    replacements.clear();
}

void TextLogger::ResetAutoSizes()
{
    Converter->ResetAutoSizes();
}


void TextLogger::Log( Domain& domain, Verbosity verbosity, Boxes& logables, ScopeInfo& scope )
{
    // we store the current msgBuf length and reset the buffer to this length when exiting.
    // This allows recursive calls! Recursion might happen with the evaluation of the
    // logables (in the next line).
    integer msgBufStartLength= msgBuf.Length();
    Converter->ConvertObjects( msgBuf, logables );

    // replace strings in message
    for ( size_t i= 0; i < replacements.size() ; i+= 2 )
        msgBuf.SearchAndReplace( replacements[i],
                                 replacements[i + 1], msgBufStartLength );

    // clear log buffer and write meta info
    logBuf.Reset();
    AutoSizes.Start();
    MetaInfo->Write( *this, logBuf, domain, verbosity, scope );
    logBuf._NC( ESC::EOMETA );

    // check for empty messages
    if ( msgBuf.Length() == msgBufStartLength )
    {
        // log empty msg and quit
        logBuf._NC( FmtMsgSuffix );
        ALIB_IF_THREADS( if (usesStdStreams) SmartLock::StdOutputStreams.Acquire(ALIB_CALLER_PRUNED); )
            logText( domain, verbosity, logBuf, scope, -1 );
        ALIB_IF_THREADS( if (usesStdStreams) SmartLock::StdOutputStreams.Release();                   )
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
            cntReplacements+=    msgBuf.SearchAndReplace( A_CHAR("\r\n"), replacement, msgBufStartLength );
            cntReplacements+=    msgBuf.SearchAndReplace( A_CHAR("\r"),   replacement, msgBufStartLength );
            cntReplacements+=    msgBuf.SearchAndReplace( A_CHAR("\n"),   replacement, msgBufStartLength );
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

        // now do the logging by calling our derived class's logText
        ALIB_IF_THREADS( if (usesStdStreams) SmartLock::StdOutputStreams.Acquire(ALIB_CALLER_PRUNED); )
            logText( domain, verbosity, logBuf, scope, -1 );
        ALIB_IF_THREADS( if (usesStdStreams) SmartLock::StdOutputStreams.Release();                   )

        // stop here
        msgBuf.ShortenTo( msgBufStartLength );
        return;
    }

    // multiple line output
    size_t   qtyTabStops= AutoSizes.ActualIndex;
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
                    --actEnd;
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

                ALIB_IF_THREADS( if (usesStdStreams) SmartLock::StdOutputStreams.Acquire(ALIB_CALLER_PRUNED);)
                    logText( domain, verbosity, logBuf, scope, -1 );
                ALIB_IF_THREADS( if (usesStdStreams) SmartLock::StdOutputStreams.Release();                  )

                // stop here
                msgBuf.ShortenTo( msgBufStartLength );
                return;
            }

            // store actual end
            actEnd= msgBuf.Length();
        }

        // found a delimiter

        // signal start of multi line log
        if ( lineNo == 0 )
        {
            ALIB_IF_THREADS( if (usesStdStreams) SmartLock::StdOutputStreams.Acquire(ALIB_CALLER_PRUNED); )
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
            lbLenBeforeMsgPart= 0;
        }

        // clear meta information?
        if ( MultiLineMsgMode == 2 )
        {
            if (lineNo != 0 )
            {
                logBuf.Reset( ESC::EOMETA );
                AutoSizes.ActualIndex=  qtyTabStops;
            }
        }
        // reset logBuf length to marked position
        else
        {
            logBuf.ShortenTo( lbLenBeforeMsgPart );
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
        ++lineNo;
    }

    // signal end of multi line log
    if ( lineNo > 0 )
    {
        notifyMultiLineOp( Phase::End );
        ALIB_IF_THREADS( if (usesStdStreams) SmartLock::StdOutputStreams.Release(); )
    }

    msgBuf.ShortenTo( msgBufStartLength );
}

}}}}}// namespace [aworx::lib::lox::detail::textlogger]


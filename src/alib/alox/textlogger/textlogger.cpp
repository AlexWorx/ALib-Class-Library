// #################################################################################################
//  alib::lox::detail - ALox Logging Library
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
#include "alib/alox/alox.prepro.hpp"

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.ALox.Impl;
    import   ALib.Lang;
    import   ALib.EnumOps;
    import   ALib.Strings;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Format.FormatterPythonStyle;
    import   ALib.Format.FormatterJavaStyle;
    import   ALib.Variables;
    import   ALib.Camp;
    import   ALib.Camp.Base;
#else
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Format.FormatterPythonStyle.H"
#   include "ALib.Format.FormatterJavaStyle.H"
#   include "ALib.Variables.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.ALox.H"
#   include "ALib.ALox.Impl.H"
#endif
// ======================================   Implementation   =======================================
namespace alib::lox::textlogger {

// #################################################################################################
// StandardConverter
// #################################################################################################
StandardConverter::StandardConverter()
{
    auto* firstLevelFormatter= new FormatterPythonStyle();
    firstLevelFormatter->Next.InsertDerived<FormatterJavaStyle>();

    Formatters.emplace_back( firstLevelFormatter );
    cntRecursion= -1;
}

StandardConverter::~StandardConverter()
{
    ALIB_ASSERT_ERROR( cntRecursion == -1, "ALOX",
        "ALox object converter recursion counter > 0.\n"
        "Note: This error indicates, that a previous format operation (log statement) contained\n"
        "      corrupt format values, which caused the formatter to behave undefined, including\n"
        "      the corruption of the execution stack of ALox logging."                             )
    for( auto* elem : Formatters )
        delete elem;
}

void StandardConverter::ConvertObjects( AString& target, BoxesMA& logables  )
{
    ++cntRecursion;

        ALIB_ASSERT_WARNING( cntRecursion < 5, "ALOX", "Logging recursion depth >= 5" )

        // get a formatter. We use a clone per recursion depth!
        // So, did we have this depth already before? If not, create a new set of formatters formatter
        if( size_t( cntRecursion ) >= Formatters.size()  )
        {
            // create a pair of recursion formatters
            Formatter* recursionFormatter= new FormatterPythonStyle();
            recursionFormatter->Next.InsertDerived<FormatterJavaStyle>();
            recursionFormatter->CloneSettings( *Formatters[0] );
            Formatters.emplace_back( recursionFormatter );
        }

        Formatter* formatter= Formatters[size_t( cntRecursion )];

        try
        {
            formatter->FormatArgs( target, logables );
        }
        catch(Exception& e )
        {
            target << ALOX.GetResource("TLFmtExc");
            ALIB_LOCK_RECURSIVE_WITH( format::Formatter::DefaultLock )
            e.Format( target );
        }

    --cntRecursion;
}

void StandardConverter::SetAutoSizes( AutoSizes* autoSizes )
{
    FormatterPythonStyle* fmtPS= dynamic_cast<FormatterPythonStyle*>( Formatters[0] );
    if (fmtPS != nullptr )
        fmtPS->Sizes= autoSizes;
}
    
AutoSizes* StandardConverter::GetAutoSizes()
{
    FormatterPythonStyle* fmtPS= dynamic_cast<FormatterPythonStyle*>( Formatters[0] );
    if (fmtPS != nullptr )
        return fmtPS->Sizes;
    return nullptr;
}

void StandardConverter::ResetAutoSizes()
{
    FormatterPythonStyle* fmtPS;
    for( auto* elem : Formatters )
        if ( (fmtPS= dynamic_cast<FormatterPythonStyle*>( elem )) != nullptr )
            fmtPS->Sizes->Reset();
}

// #################################################################################################
// MetaInfo
// #################################################################################################
void TextLogger::writeMetaInfo( AString& buf, detail::Domain& domain, Verbosity verbosity,
                                detail::ScopeInfo& scope )
{
    // check
    auto& fmt= varFormatMetaInfo.Get<FormatMetaInfo>();
    if ( fmt.Format.IsEmpty() )
        return;

    // clear DateTime singleton
    callerDateTime.Year= (std::numeric_limits<int>::min)();

    Substring format( fmt.Format );
    for(;;)
    {
        // get next and log substring between commands
        integer idx= format.IndexOf( '%' );
        if ( idx >= 0 )
        {
            format.ConsumeChars<NC, lang::CurrentData::Keep>( idx, buf, 1 );
            processVariable( domain.FullPath, verbosity, scope, buf, format );
        }
        else
        {
            buf._<NC>( format );
            break;
        }
    }
}

void TextLogger::processVariable( const NString&     domainPath,
                                  Verbosity          verbosity,
                                  detail::ScopeInfo& scope,
                                  AString&           dest,
                                  Substring&         variable       )

{
    // process commands
    auto& fmt=          varFormatMetaInfo .Get<FormatMetaInfo>();
    auto& autoSizes=    varFormatAutoSizes.Get<FormatAutoSizes>();
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
                        val= GetFormatOther().NoSourceFileInfo;
                } break;

                case 'p':   // Sp: trimmed path
                {
                    integer previousLength= dest.Length();
                    scope.GetTrimmedPath( dest );
                    if( dest.Length() != previousLength )
                        return;
                    val= GetFormatOther().NoSourceFileInfo;
                } break;

                case 'F':   // file name
                {
                    val= scope.GetFileName();
                    if ( val.IsEmpty() )
                        val= GetFormatOther().NoSourceFileInfo;
                } break;

                case 'f':   // file name without extension
                {
                    val= scope.GetFileNameWithoutExtension();
                    if ( val.IsEmpty() )
                        val= GetFormatOther().NoSourceFileInfo;
                } break;


                case 'M':   // method name
                {
                    val= scope.GetMethod();
                    if ( val.IsEmpty() )
                        val= GetFormatOther().NoMethodInfo;
                } break;

                case 'L':  // line number
                {
                    dest._<NC>( scope.GetLineNumber() );
                    return;
                }

                default:
                {
                    ALIB_ASSERT_WARNING( FormatWarningOnce, "ALOX",
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
                if ( GetFormatDate().Date.Equals<NC>( A_CHAR("yyyy-MM-dd") ) )
                {
                    dest._<NC>( alib::Dec( callerDateTime.Year,     4 ) )._<NC>( '-' )
                        ._<NC>( alib::Dec( callerDateTime.Month,    2 ) )._<NC>( '-' )
                        ._<NC>( alib::Dec( callerDateTime.Day,      2 ) );
                }
                // user-defined format
                else
                    callerDateTime.Format( GetFormatDate().Date, dest  );

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
                if ( GetFormatDate().TimeOfDay.Equals<NC>( A_CHAR("HH:mm:ss") ) )
                {
                    dest._<NC>( alib::Dec(callerDateTime.Hour,    2) )._<NC>( ':' )
                        ._<NC>( alib::Dec(callerDateTime.Minute,  2) )._<NC>( ':' )
                        ._<NC>( alib::Dec(callerDateTime.Second,  2) );
                }

                // user-defined format
                else
                    callerDateTime.Format( GetFormatDate().TimeOfDay, dest  );
            }

            // %TC: Time elapsed since created
            else if ( c2 == 'C' )
            {
                auto elapsedTime= scope.GetTimeStamp() - TimeOfCreation;
                auto elapsedSecs= elapsedTime.InAbsoluteSeconds();
                CalendarDuration  elapsed( elapsedTime );

                // determine number of segments to write and match this to recent (autosizes) value
                int timeSize=   elapsedSecs >= 24*3600  ? 6
                              : elapsedSecs >= 10*3600  ? 5
                              : elapsedSecs >= 3600     ? 4
                              : elapsedSecs >= 10*60    ? 3
                              : elapsedSecs >= 60       ? 2
                              : elapsedSecs >= 9        ? 1
                              :                           0;
                timeSize= int(autoSizes.Main.Next( AutoSizes::Types::Field, timeSize, 0 ));


                if ( timeSize >= 4 )  dest._<NC>( elapsed.Days  )._<NC>( GetFormatDate().ElapsedDays );
                if ( timeSize >= 3 )  dest._<NC>( alib::Dec(elapsed.Hours  ,  timeSize >= 5 ?  2 : 1 ) )._<NC>( ':' );
                if ( timeSize >= 2 )  dest._<NC>( alib::Dec(elapsed.Minutes,  timeSize >= 3 ?  2 : 1 ) )._<NC>( ':' );
                dest._<NC>( alib::Dec(elapsed.Seconds,  timeSize >= 1 ? 2 : 1)                         )._<NC>( '.' );
                dest._<NC>( alib::Dec(elapsed.Milliseconds,  3) );
            }

            // %TL: Time elapsed since last log call
            else if ( c2 == 'L' )
                writeTimeDiff( dest, scope.GetTimeStamp().Since( TimeOfLastLog ).InNanoseconds() );

            else
            {
                ALIB_ASSERT_WARNING( FormatWarningOnce, "ALOX",
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
                #if !ALIB_SINGLE_THREADED
                    const String& threadName= scope.GetThreadNameAndID(nullptr);
                #else
                    String msg( A_CHAR("SINGLE_THREADED") );
                    const String& threadName= msg;
                #endif
                dest._<NC>( Field( threadName,
                                   autoSizes.Main.Next(
                                       AutoSizes::Types::Field, threadName.Length(), 0),
                                   lang::Alignment::Center ) );
            }
            else if ( c2 == 'I' )   // %tI: thread ID
            {
                String32 threadID;
                #if !ALIB_SINGLE_THREADED
                    threadID._( scope.GetThreadID() );
                #else
                    threadID << "-1";
                #endif
                dest._<NC>( Field( threadID,
                                              autoSizes.Main.Next(
                                                  AutoSizes::Types::Field, threadID  .Length(), 0),
                                              lang::Alignment::Center ) );
            }
            else
            {
                ALIB_ASSERT_WARNING( FormatWarningOnce, "ALOX",
                                     "Unknown format variable '%t{}' (only one warning)", c2  )
                ALIB_DBG( FormatWarningOnce= true; )
            }
            return;
        }

        case 'L':
        {
            c2= variable.ConsumeChar();
                 if ( c2 == 'G' )     dest._<NC>( GetName() );
            else if ( c2 == 'X' )     dest._<NC>( scope.GetLoxName() );
            else
            {
                ALIB_ASSERT_WARNING( FormatWarningOnce, "ALOX",
                                     "Unknown format variable '%L{}' (only one warning)", c2  )
                ALIB_DBG( FormatWarningOnce= true; )
            }
            return;
        }

        case 'P':
        {
            dest._<NC>( ProcessInfo::Current().Name );
            return;
        }

        case 'V':
            dest._<NC>(  verbosity == Verbosity::Error     ? fmt.VerbosityError
                          : verbosity == Verbosity::Warning   ? fmt.VerbosityWarning
                          : verbosity == Verbosity::Info      ? fmt.VerbosityInfo
                          :                                     fmt.VerbosityVerbose    );
            return;

        case 'D':
        {
            dest._( Field( domainPath,
                                   autoSizes.Main.Next(
                                             AutoSizes::Types::Field, domainPath.Length(), 0 ),
                                   lang::Alignment::Left ) );
            return;
        }

        case '#':
            dest._<NC>( alib::Dec( CntLogs, GetFormatOther().LogNumberMinDigits ) );
            return;

        // A: Auto tab
        case 'A':
        {
            // read extra space from format string
            integer extraSpace;
            if( !variable.ConsumeDecDigits( extraSpace ) )
                extraSpace= 1;
            integer currentLength= dest.WStringLength();
            integer tabPos= autoSizes.Main.Next(
                                            AutoSizes::Types::Tabstop, currentLength, extraSpace);
            dest.InsertChars(' ', tabPos - currentLength );

            return;
        }

        case 'N':
            dest._<NC>( GetName() );
            return;

        default:
            ALIB_ASSERT_WARNING( FormatWarningOnce, "ALOX",
                   "Unknown format character '{}' (only one warning)", *( variable.Buffer() -1 ) )
            ALIB_DBG( FormatWarningOnce= true; )
            return;
   }// switch
}

void TextLogger::writeTimeDiff( AString& buf, int64_t diffNanos )
{
    auto& td= GetFormatTimeDiff();

    // unmeasurable?
    if ( diffNanos < td.Minimum )
    {
        buf._<NC>( td.None );
        return;
    }

    if ( diffNanos < 1000 )
    {
        buf._<NC>( alib::Dec( diffNanos, 3 ) )._<NC>( td.Nanos );
        return;
    }

    // we continue with micros
    int64_t diffMicros= diffNanos / 1000L;

    // below 1000 microseconds?
    if ( diffMicros < 1000 )
    {
        buf._<NC>( alib::Dec( diffMicros, 3 ) );
        buf._<NC>( td.Micros );
        return;
    }

    // below 1000 ms?
    if ( diffMicros < 1000000 )
    {
        buf._<NC>( alib::Dec( (diffMicros / 1000), 3 ) )._<NC>( td.Millis );
        return;
    }


    // below 10 secs (rounded) ?
    if ( diffMicros < 9995000 )
    {
        // convert to hundredth of secs
        int64_t hundredthSecs=  ((diffMicros / 1000) + 5) / 10;

        // print two digits after dot x.xx
        buf._<NC>( alib::Dec( (hundredthSecs / 100), 1 ) )
           ._<NC>( '.' )
           ._<NC>( alib::Dec( (hundredthSecs % 100), 2 ) )
           ._<NC>( td.Secs );
        return;
    }

    // convert to tenth of secs
    int64_t tenthSecs=  ((diffMicros / 10000) + 5) / 10 ;

    // below 100 secs ?
    if ( tenthSecs < 1000 )
    {
        // print one digits after dot xx.x (round value by adding 5 hundredth)
        buf._<NC>( alib::Dec( ( tenthSecs / 10 ), 2 ) )
           ._<NC>( '.' )
           ._<NC>( alib::Dec( ( tenthSecs % 10 ), 1 ) )
           ._<NC>( td.Secs );
        return;
    }

    //     below 10 mins ?
    if ( tenthSecs < 6000 )
    {
        // convert to hundredth of minutes
        int64_t hundredthMins=  tenthSecs / 6;

        // print two digits after dot x.xx
        buf._<NC>( alib::Dec( (hundredthMins / 100), 1 ) )
           ._<NC>( '.' )
           ._<NC>( alib::Dec( (hundredthMins % 100), 2 ) )
           ._<NC>( td.Mins );
        return;
    }

    // convert to tenth of minutes
    int64_t tenthMins=  tenthSecs / 60;

    // below 100 mins ?
    if ( tenthMins < 1000 )
    {
        // print one digits after dot xx.x (round value by adding 5 hundredth)
        buf._<NC>( alib::Dec( (tenthMins / 10), 2 ) )
           ._<NC>( '.' )
           ._<NC>( alib::Dec( (tenthMins % 10), 1 ) )
           ._<NC>( td.Mins );
        return;
    }

    // below ten hours?
    if ( tenthMins < 6000 )
    {
        // convert to hundredth of hours
        int64_t hundredthHours=  tenthMins / 6;

        // print two digits after dot x.xx
        buf._<NC>( alib::Dec( (hundredthHours / 100), 1 ) )
           ._<NC>( '.' )
           ._<NC>( alib::Dec( (hundredthHours % 100), 2 ))
           ._<NC>( td.Hours );
        return;
    }

    // convert to tenth of minutes
    int64_t tenthHours=  tenthMins / 60;

    // below 10 hours ?
    if ( tenthHours < 1000 )
    {
        // print two digits after dot x.xx
        buf._<NC>( alib::Dec( (tenthHours / 10), 2 ) )
           ._<NC>( '.' )
           ._<NC>( alib::Dec( (tenthHours % 10), 1 ) )
           ._<NC>( td.Hours );
        return;
    }

    // below 100 hours ?
    if ( tenthHours < 1000 )
    {
        // print one digits after dot xx.x (round value by adding 5 hundredth)
        buf._<NC>( alib::Dec( (tenthHours / 10), 2 ) )
           ._<NC>( '.' )
           ._<NC>( alib::Dec( ((tenthHours / 10) % 10), 1 ) )
           ._<NC>( td.Hours );
        return;
    }

    // convert to hundredth of days
    int64_t hundredthDays=  tenthHours * 10 / 24;

    // below 10 days ?
    if ( hundredthDays < 1000 )
    {
        // print two digits after dot x.xx
        buf._<NC>( alib::Dec( (hundredthDays / 100), 1 ) )
           ._<NC>( '.' )
           ._<NC>( alib::Dec( (hundredthDays % 100), 2 ) )
           ._<NC>( td.Days );
        return;
    }

    // 10 days or more (print days plus one digit after the comma)
    // print one digits after dot xx.x (round value by adding 5 hundredth)
    buf ._<NC>( alib::Dec( (hundredthDays / 100), 2 ) )
        ._<NC>( '.' )
        ._<NC>( alib::Dec( ((hundredthDays / 10) % 10), 1 ) )
        ._<NC>( td.Days );
}


// #################################################################################################
// TextLogger
// #################################################################################################
TextLogger::TextLogger( const NString& pName, const NString& typeName, bool  pUsesStdStreams  )
: Logger( pName, typeName )
, usesStdStreams( pUsesStdStreams )
, varFormatMetaInfo (variables::CampVariable(alib::ALOX))
, varFormatDateTime (variables::CampVariable(alib::ALOX))
, varFormatTimeDiff (variables::CampVariable(alib::ALOX))
, varFormatMultiLine(variables::CampVariable(alib::ALOX))
, varFormatOther    (variables::CampVariable(alib::ALOX))
, varFormatAutoSizes(variables::CampVariable(alib::ALOX))
, varReplacements   (variables::CampVariable(alib::ALOX))
{
    logBuf.SetBuffer( 256 );
    msgBuf.SetBuffer( 256 );
}

TextLogger::~TextLogger()
{
    if (Converter)
        delete Converter;
    ALIB_ASSERT( msgBuf.IsEmpty(), "ALOX" )
}

void   TextLogger::AcknowledgeLox( detail::LoxImpl* , lang::ContainerOp op )
{
    // ---------------  insert ------------------
    if( op == lang::ContainerOp::Insert )
    {
        if ( Converter == nullptr )
            Converter= new textlogger::StandardConverter();

        // Variable AUTO_SIZES: use last session's values
        {ALIB_LOCK_WITH(ALOX.GetConfig())
            varFormatAutoSizes.Declare(Variables::AUTO_SIZES, Name );
            (void) varFormatAutoSizes.Define();
            Converter->SetAutoSizes( &varFormatAutoSizes.Get<FormatAutoSizes>().LogMessage );
        }

        // Variable  <name>_FORMAT / <typeName>_FORMAT:
        {ALIB_LOCK_WITH(ALOX.GetConfig())
            const Declaration* variableDecl= Declaration::Get( Variables::FORMAT );
            const Declaration* privateDecl= ALOX.GetConfig()->StoreDeclaration(variableDecl, GetName()     );

            if(    !varFormatMetaInfo.Try( privateDecl )
                && !varFormatMetaInfo.Try( ALOX.GetConfig()->StoreDeclaration(variableDecl, GetTypeName() )    ) )
            {
                varFormatMetaInfo.Declare( privateDecl );
                ALIB_ASSERT_ERROR(varFormatMetaInfo.IsDefined(), "ALOX",
                    "Mandatory (usually resourced) default value is missing for variable \"{}\".",
                    &varFormatMetaInfo)
            }
        }

        // Variable  <name>_FORMAT_DATE_TIME / <typeName>_FORMAT_DATE_TIME:
        {ALIB_LOCK_WITH(ALOX.GetConfig())
            auto* variableDecl= Declaration::Get( Variables::FORMAT_DATE_TIME );
            auto* privateDecl= ALOX.GetConfig()->StoreDeclaration(variableDecl, GetName()     );
            if(    !varFormatDateTime.Try( privateDecl )
                && !varFormatDateTime.Try( ALOX.GetConfig()->StoreDeclaration(variableDecl, GetTypeName() )    )  )
            {
                varFormatDateTime.Declare( privateDecl );

                ALIB_ASSERT_ERROR(varFormatDateTime.IsDefined(), "ALOX",
                   "Mandatory (usually resourced) default value is missing for variable \"{}\".",
                   varFormatDateTime)
            }
        }

        // Variable  <name>FORMAT_TIME_DIFF / <typeName>FORMAT_TIME_DIFF:
        {ALIB_LOCK_WITH(ALOX.GetConfig())
            auto* variableDecl= Declaration::Get( Variables::FORMAT_TIME_DIFF );
            auto* privateDecl= ALOX.GetConfig()->StoreDeclaration(variableDecl, GetName()     );
            if(    !varFormatTimeDiff.Try( privateDecl )
                && !varFormatTimeDiff.Try( ALOX.GetConfig()->StoreDeclaration(variableDecl, GetTypeName() )    ) )
            {
                varFormatTimeDiff.Declare( privateDecl );
                ALIB_ASSERT_ERROR(varFormatTimeDiff.IsDefined(), "ALOX",
                  "Mandatory (usually resourced) default value is missing for variable \"{}\".",
                  varFormatTimeDiff)
            }
        }

        // Variable  <name>FORMAT_MULTILINE / <typeName>FORMAT_MULTILINE:
        {ALIB_LOCK_WITH(ALOX.GetConfig())
            auto* variableDecl= Declaration::Get( Variables::FORMAT_MULTILINE );
            auto* privateDecl= ALOX.GetConfig()->StoreDeclaration(variableDecl, GetName()     );
            if(    !varFormatMultiLine.Try( privateDecl )
                && !varFormatMultiLine.Try( ALOX.GetConfig()->StoreDeclaration(variableDecl, GetTypeName() )    ) )
            {
                varFormatMultiLine.Declare( privateDecl );
                ALIB_ASSERT_ERROR(varFormatMultiLine.IsDefined(), "ALOX",
                  "Mandatory (usually resourced) default value is missing for variable \"{}\".",
                  varFormatMultiLine)
            }
        }

        // Variable  <name>FORMAT_OTHER / <typeName>FORMAT_OTHER:
        {ALIB_LOCK_WITH(ALOX.GetConfig())
            auto* variableDecl= Declaration::Get( Variables::FORMAT_OTHER );
            auto* privateDecl= ALOX.GetConfig()->StoreDeclaration(variableDecl, GetName() );
            if(    !varFormatOther.Try( privateDecl )
                && !varFormatOther.Try( ALOX.GetConfig()->StoreDeclaration(variableDecl, GetTypeName() ) ) )
            {
                varFormatOther.Declare( privateDecl );
                ALIB_ASSERT_ERROR(varFormatOther.IsDefined(), "ALOX",
                   "Mandatory (usually resourced) default value is missing for variable \"{}\".",
                   varFormatOther )
            }
        }

        // Variable  <name>FORMAT_REPLACEMENTS / <typeName>FORMAT_REPLACEMENTS:
        {ALIB_LOCK_WITH(ALOX.GetConfig())
            auto* variableDecl= Declaration::Get( Variables::REPLACEMENTS );
            auto* privateDecl = ALOX.GetConfig()->StoreDeclaration(variableDecl, GetName() );
            if(    !varReplacements.Try( privateDecl)
                && !varReplacements.Try( ALOX.GetConfig()->StoreDeclaration(variableDecl, GetTypeName())) )
            {
                varReplacements.Declare(privateDecl);
            }

            // if not defined, create the empty variable. For one, this way it is not needed to be
            // checked before access, and furthermore this allows it to appear in config files.
            if( !varReplacements.IsDefined() )
                (void) varReplacements.Define(Priority::DefaultValues - 1);
        }
    }
}


void TextLogger::SetReplacement( const String& searched, const String& replacement )
{
    auto& replacements= GetReplacements().Pairs;
    // if exists, replace replacement
    for( auto it= replacements.begin(); it < replacements.end(); it+= 2)
        if ( it->Equals<NC>( searched ) )
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
        replacements.insert( replacements.end(), AStringPA(replacements.get_allocator().GetAllocator()) );
        replacements.back() << searched;
        replacements.insert( replacements.end(), AStringPA(replacements.get_allocator().GetAllocator()) );
        replacements.back() << replacement;
    }
}

void TextLogger::ClearReplacements()
{
    GetReplacements().Pairs.clear();
}

void TextLogger::ResetAutoSizes()
{
    Converter->ResetAutoSizes();
}


void TextLogger::Log( detail::Domain& domain, Verbosity verbosity, BoxesMA& logables,
                      detail::ScopeInfo& scope )
{
    // we store the current msgBuf length and reset the buffer to this length when exiting.
    // This allows recursive calls! Recursion might happen with the evaluation of the
    // logables (in the next line).
    StringLengthResetter msgBufResetter(msgBuf);
    Converter->ConvertObjects( msgBuf, logables );

    // replace strings in message
    auto& replacements= GetReplacements().Pairs;
    for ( size_t i= 0; i < replacements.size() ; i+= 2 )
        msgBuf.SearchAndReplace( replacements[i],
                                 replacements[i + 1], msgBufResetter.OriginalLength() );

    // get auto-sizes and set write protected in case someone wrote the variable with higher
    // priority than we do.
    auto& autoSizes=    varFormatAutoSizes.Get<FormatAutoSizes>();
    if( varFormatAutoSizes.GetPriority() > Priority::Session )
        autoSizes.Main      .WriteProtected=
        autoSizes.LogMessage.WriteProtected= true;

    // clear log buffer and write meta info
    logBuf.Reset();
    autoSizes.Main.Restart();
    writeMetaInfo( logBuf, domain, verbosity, scope );
    logBuf._<NC>( ESC::EOMETA );

    // check for empty messages
    auto& fmt=       varFormatMetaInfo.Get<FormatMetaInfo>();
    if ( msgBuf.Length() == msgBufResetter.OriginalLength() )
    {
        // log empty msg and quit
        logBuf._<NC>( fmt.MsgSuffix );
        if ( usesStdStreams )
        {ALIB_LOCK_WITH( threads::STD_IOSTREAMS_LOCK )
            logText( domain, verbosity, logBuf, scope, -1 );
        }
        else
            logText( domain, verbosity, logBuf, scope, -1 );
        return;
    }
    
    // single line output
    auto& multiLine= varFormatMultiLine.Get<FormatMultiLine>();
    if ( multiLine.Mode == 0 )
    {
        // replace line separators
        integer cntReplacements=0;
        if ( multiLine.Delimiter.IsNotNull() )
            cntReplacements+=    msgBuf.SearchAndReplace( multiLine.Delimiter, multiLine.DelimiterReplacement, msgBufResetter.OriginalLength() );
        else
        {
            String replacement= multiLine.DelimiterReplacement;
            cntReplacements+=    msgBuf.SearchAndReplace( A_CHAR("\r\n"), replacement, msgBufResetter.OriginalLength() );
            cntReplacements+=    msgBuf.SearchAndReplace( A_CHAR("\r"),   replacement, msgBufResetter.OriginalLength() );
            cntReplacements+=    msgBuf.SearchAndReplace( A_CHAR("\n"),   replacement, msgBufResetter.OriginalLength() );
        }

        // append msg to logBuf
        if ( cntReplacements == 0 )
        {
            logBuf._<NC>( msgBuf, msgBufResetter.OriginalLength(), msgBuf.Length() - msgBufResetter.OriginalLength() );
        }
        else
        {
            logBuf._<NC>( multiLine.Prefix );
            logBuf._<NC>( msgBuf, msgBufResetter.OriginalLength(), msgBuf.Length() - msgBufResetter.OriginalLength() );
            logBuf._<NC>( multiLine.Suffix );
        }
        logBuf._<NC>( fmt.MsgSuffix );

        // now do the logging by calling our derived class's logText
        if ( usesStdStreams )
        {ALIB_LOCK_WITH( threads::STD_IOSTREAMS_LOCK )
            logText( domain, verbosity, logBuf, scope, -1 );
        }
        else
            logText( domain, verbosity, logBuf, scope, -1 );
    }

    // multiple line output
    auto     prevIndex=   autoSizes.Main.ActualIndex;
    integer  actStart=    msgBufResetter.OriginalLength();
    int      lineNo=      0;
    integer  lbLenBeforeMsgPart= logBuf.Length();

    // loop over lines in msg
    while ( actStart < msgBuf.Length() )
    {
        // find next end
        integer delimLen;
        integer actEnd;

        // no delimiter given: search '\n' and then see if it is "\r\n" in fact
        if (multiLine.Delimiter.IsEmpty() )
        {
            delimLen= 1;

            actEnd= msgBuf.IndexOf<NC>( '\n', actStart );
            if( actEnd > actStart )
            {
                if( msgBuf.CharAt<NC>(actEnd - 1) == '\r' )
                {
                    --actEnd;
                    delimLen= 2;
                }
            }
        }
        else
        {
            delimLen=  multiLine.Delimiter.Length();
            actEnd=    msgBuf.IndexOf<NC>( multiLine.Delimiter, actStart );
        }

        // not found a delimiter? - log the rest
        if ( actEnd < 0 )
        {
            // single line?
            if ( lineNo == 0 )
            {
                logBuf._<NC>( msgBuf, msgBufResetter.OriginalLength(), msgBuf.Length() - msgBufResetter.OriginalLength() );
                logBuf._<NC>( fmt.MsgSuffix );

                if ( usesStdStreams )
                {ALIB_LOCK_WITH( threads::STD_IOSTREAMS_LOCK )
                    logText( domain, verbosity, logBuf, scope, -1 );
                }
                else
                    logText( domain, verbosity, logBuf, scope, -1 );

                // stop here
                goto LOG_END;
            }

            // store actual end
            actEnd= msgBuf.Length();
        }

        // found a delimiter

        // signal start of multi line log
        if ( lineNo == 0 )
        {
            if ( usesStdStreams)
                {ALIB_LOCK_WITH( threads::STD_IOSTREAMS_LOCK )
                    notifyMultiLineOp( lang::Phase::Begin );
                }
                else
                    notifyMultiLineOp( lang::Phase::Begin );
        }

        // in mode 3, 4, meta info is deleted
        if ( lineNo == 0 && ( multiLine.Mode == 3 || multiLine.Mode == 4 ) )
        {
            // log headline in mode 3
            if ( multiLine.Mode == 3 )
            {
                logBuf._<NC>( multiLine.Headline );
                autoSizes.Main.ActualIndex=  prevIndex;
                logText( domain, verbosity, logBuf, scope, 0 );
            }

            // remember zero as offset
            lbLenBeforeMsgPart= 0;
        }

        // clear meta-information?
        if ( multiLine.Mode == 2 )
        {
            if (lineNo != 0 )
            {
                logBuf.Reset(ESC::EOMETA);
                autoSizes.Main.ActualIndex=  prevIndex;
            }
        }
        // reset logBuf length to marked position
        else
        {
            logBuf.ShortenTo( lbLenBeforeMsgPart );
            autoSizes.Main.ActualIndex=  prevIndex;
        }

        // append message and do the log
        logBuf._<NC>( multiLine.Prefix );
          logBuf._<NC>( msgBuf,  actStart, actEnd - actStart  );
        logBuf._<NC>( multiLine.Suffix );
        actStart= actEnd + delimLen;
        if ( actStart >= msgBuf.Length() )
            logBuf._<NC>( fmt.MsgSuffix );
        logText( domain, verbosity, logBuf, scope, lineNo );

        // next
        ++lineNo;
    }

    // signal end of multi line log
    if ( lineNo > 0 )
    {
        if ( usesStdStreams)
            {ALIB_LOCK_WITH( threads::STD_IOSTREAMS_LOCK )
                notifyMultiLineOp( lang::Phase::End );
            }
            else
                notifyMultiLineOp( lang::Phase::End );
    }

    LOG_END:
    // In case of changes, re-define the auto-sizes variable.
    // This might trigger a listener that monitors this session-type variable
    if(    autoSizes.Main      .IsChanged()
        || autoSizes.LogMessage.IsChanged()  )
        (void) varFormatAutoSizes.Define( Priority::Session );
} // TextLogger::Log()

} // namespace [alib::lox::textlogger]



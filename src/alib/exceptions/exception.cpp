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
#include <vector>
#include <algorithm>
#include <any>
#include "alib/boxing/boxing.prepro.hpp"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Exceptions;
    import   ALib.Lang;
    import   ALib.Strings;
    import   ALib.Strings.Tokenizer;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
#  if ALIB_SYSTEM
    import   ALib.System;
#  endif
#  if ALIB_FORMAT
    import   ALib.Format;
#  endif
    import   ALib.Format.Paragraphs;
#  if ALIB_CAMP
    import   ALib.Camp.Base;
#  endif
#else
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.Strings.Tokenizer.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.System.H"
#   include "ALib.Format.H"
#   include "ALib.Format.Paragraphs.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.Exceptions.H"
#endif
// ======================================   Implementation   =======================================
ALIB_BOXING_VTABLE_DEFINE( alib::exceptions::Exception*, vt_alib_exception )


namespace alib::exceptions {

void ERException::Parse()
{
    enumrecords::bootstrap::EnumRecordParser::Get( ERSerializable::EnumElementName       );
                                             ERSerializable::MinimumRecognitionLength= 0;
    enumrecords::bootstrap::EnumRecordParser::Get( DescriptionOrItsResourceName   , true );
}

Message* Exception::allocMessageLink()
{
    // find pointer to the last entry pointer;
    detail::ExceptionEntry** tail= &**this;
    while(*tail != nullptr)
        tail= &(*tail)->next;

    *tail= GetAllocator()().Alloc<detail::ExceptionEntry>();
    (*tail)->next= nullptr;

    return &(*tail)->message;
}


void Exception::finalizeMessage( Message* message, bool hasRecord, ResourcePool* pool, const NString& category )
{
    message->CloneAll();

    if( hasRecord )
    {
        #if ALIB_DEBUG
        {
            auto* tryRecord= boxing::TryRecord<ERException>(message->Type);
            if( tryRecord == nullptr )
            {
                std::vector<std::pair<integer,const void*>> recordList;
                for( auto& record : enumrecords::detail::getInternalRecordMap() )
                    if( record.first.RTTI == message->Type.TypeID() )
                        recordList.push_back( std::pair<integer,const void*>(record.first.Element, record.second) );
                if( recordList.size() == 0)
                {
                    ALIB_ERROR( "EXCEPT", "No enum records defined for exception enumeration "
                                          "type {!Q<>}.", message->Type.TypeID().name() )
                }
                else
                {
                    std::sort( recordList.begin(), recordList.end(),
                    [] (std::pair<integer,const void*>& a, std::pair<integer,const void*>& b )
                       {
                           return a.first < b.first;
                       });
                    std::vector<std::any> args; args.reserve(32);
                    args.emplace_back( "Enum record {} not found for exception enumeration type {}.\n"
                                       "The following records have been found:\n" );
                    args.emplace_back( message->Type.Integral() );
                    args.emplace_back( &message->Type.TypeID()   );

                    for( auto& pair : recordList )
                    {
                        args.emplace_back( "  {}: {}\n" );
                        args.emplace_back( pair.first );
                        args.emplace_back( reinterpret_cast<const ERException*>( pair.second )->EnumElementName );
                    }
                    assert::raise( ALIB_CALLER_PRUNED, 1, "EXCEPT", args );
                }
            }
        }
        #endif
        const auto& enumRecord= boxing::GetRecord<ERException>(message->Type);
        if( pool == nullptr )
            message->emplace( message->begin(),
                              enumRecord.DescriptionOrItsResourceName );
        else
            message->emplace( message->begin(),
                              pool->Get( category,
                                         enumRecord.DescriptionOrItsResourceName
                                              ALIB_DBG(, true)                         )  );
    }
}


Message&   Exception::Back() const
{
    auto* result= **this;
    while( result->next != nullptr )
        result= result->next;

    return result->message;
}

int   Exception::Size() const
{
    int result= 1;
    auto* entry= **this;
    while( entry->next != nullptr )
    {
        entry= entry->next;
        ++result;
    }

    return result;
}

const Enum&   Exception::Type() const
{
    auto* entry= **this;
    Enum* result= &entry->message.Type;
    while( (entry= entry->next) != nullptr )
        if( entry->message.Type.Integral() >= 0 )
            result= &entry->message.Type;

    return *result;
}


#if ALIB_FORMAT
AString&   Exception::Format( AString& target )     const
{
    Paragraphs text(target);
    Tokenizer tknzr;
    tknzr.TrimChars= A_CHAR( "\r" );
    String1K buf;
    buf.DbgDisableBufferReplacementWarning();
    Formatter& formatter= *Formatter::Default;
    formatter.GetArgContainer();
    size_t entryNo= 1;
    for ( auto entry= begin(); entry != end(); ++entry )
    {
        text.Add( A_CHAR("{}{}: {!Q<>}"), (entry->Type.Integral() >= 0 ? 'E' : 'I'), entryNo, entry->Type );
        text.PushIndent( A_CHAR("    ") );
        try
        {
            formatter.FormatArgs( buf.Reset(), *entry );
        }
        catch( Exception& e )
        {
            #if ALIB_CAMP
                buf <<  BASECAMP.GetResource("ExcFmtExc");
            #else
                buf <<  A_CHAR("\nAn exception occurred while formatting another (!) exception:\n" );
            #endif
            e.Format( buf );
        }
        tknzr.Set( buf, '\n' );
        while( tknzr.HasNext() )
            text.Add( tknzr.Next()  );

        #if ALIB_DEBUG
            text.Add( entry->CI );
        #endif

        text.PopIndent();
        ++entryNo;
    }

    return target;
}
#endif // ALIB_FORMAT

} // namespace [alib::lang]


#if ALIB_CAMP
namespace alib::system {

Exception CreateSystemException( const CallerInfo& ci, int errNo )
{
    auto* enumRecord= enumrecords::TryRecord( SystemErrors(errNo) );
    if( enumRecord == nullptr )
        return Exception( ci, SystemErrors::UNKNOWN, errNo );

    return Exception( ci, SystemErrors(errNo),  // as exception
                          SystemErrors(errNo),  // boxing the exception's name (!)
                          errNo                    );
}
} // namespace [alib::lang]
#endif

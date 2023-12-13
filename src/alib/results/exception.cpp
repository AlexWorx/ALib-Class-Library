// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_RESULTS_EXCEPTION)
#      include "alib/results/exception.hpp"
#   endif

#   if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#      include "alib/strings/util/tokenizer.hpp"
#   endif
#   if !defined (HPP_ALIB_RESULTS_RESULTS)
#      include "alib/results/results.hpp"
#   endif

#   if !defined (HPP_ALIB_TEXT_FORMATTER_STD)
#      include "alib/text/formatterstdimpl.hpp"
#   endif

#   if !defined (HPP_ALIB_TEXT_PARAGRAPHS)
#      include "alib/text/paragraphs.hpp"
#   endif

#if !defined(HPP_ALIB_RESULTS_REPORT)
#   include "alib/results/report.hpp"
#endif

#   if !defined (HPP_ALIB_RESULTS_EXCEPTION)
#      include "alib/results/exception.hpp"
#   endif

#   if !defined (HPP_ALIB_ENUMS_RECORDPARSER)
#      include "alib/enums/recordparser.hpp"
#   endif
#   if !defined (HPP_ALIB_ENUMS_DETAIL_ENUMRECORDMAP)
#      include "alib/enums/detail/enumrecordmap.hpp"
#   endif
#endif // !defined(ALIB_DOX)


ALIB_BOXING_VTABLE_DEFINE( aworx::lib::results::Exception*, vt_alib_exception )


namespace aworx { namespace lib { namespace results {

void ERException::Parse()
{
    enums::EnumRecordParser::Get( ERSerializable::EnumElementName       );
                                  ERSerializable::MinimumRecognitionLength= 0;
    enums::EnumRecordParser::Get( DescriptionOrItsResourceName   , true );
}

Message* Exception::allocMessageLink()
{
    // find pointer to the last entry pointer;
    detail::ExceptionEntry** tail= &Self();
    while(*tail != nullptr)
        tail= &(*tail)->next;

    *tail= Allocator().Alloc<detail::ExceptionEntry>();
    (*tail)->next= nullptr;


    return &(*tail)->message;
}

void Exception::finalizeMessage( Message* message, bool hasRecord, ResourcePool* pool, const NString& category )
{
    message->CloneArguments();

    if( hasRecord )
    {
        #if ALIB_DEBUG
        {
            auto* tryRecord= message->Type.TryRecord<ERException>();
            if( tryRecord == nullptr )
            {
                std::vector<std::pair<integer,const void*>> recordList;
                for( auto& record : lib::enums::detail::getInternalRecordMap() )
                    if( record.first.RTTI == message->Type.TypeID() )
                        recordList.push_back( std::pair<integer,const void*>(record.first.Element, record.second) );
                if( recordList.size() == 0)
                {
                    ALIB_ERROR( "EXCEPT", "No enum records defined for exception enumeration type {!Q<>}.",
                                message->Type.TypeID().name() )
                }
                else
                {
                    std::sort( recordList.begin(), recordList.end(),
                    [] (std::pair<integer,const void*>& a, std::pair<integer,const void*>& b )
                       {
                           return a.first < b.first;
                       });
                    AString recordListDump;
                    auto formatter= Formatter::AcquireDefault( ALIB_CALLER_PRUNED );
                    formatter->Format( recordListDump,
                                       "Enum record {} not found for exception enumeration type {}.\n"
                                       "The following records have been found:\n",
                                       message->Type.Integral(),
                                       message->Type.TypeID()     );

                    for( auto& pair : recordList )
                        formatter->Format( recordListDump, "  {:2}: {}\n",
                                           pair.first,
                                           reinterpret_cast<const ERException*>( pair.second )->EnumElementName );
                    formatter->Release();
                    ALIB_ERROR( "EXCEPT", recordListDump )
                }
            }
        }
        #endif
        const auto& enumRecord= message->Type.GetRecord<ERException>();
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
    auto* result= Self();
    while( result->next != nullptr )
        result= result->next;

    return result->message;
}

int   Exception::Size() const
{
    int result= 1;
    auto* entry= Self();
    while( entry->next != nullptr )
    {
        entry= entry->next;
        ++result;
    }

    return result;
}

const Enum&   Exception::Type() const
{
    auto* entry= Self();
    Enum* result= &entry->message.Type;
    while( (entry= entry->next) != nullptr )
        if( entry->message.Type.Integral() >= 0 )
            result= &entry->message.Type;

    return *result;
}



AString&   Exception::Format( AString& target )     const
{
    Paragraphs text(target);
    Tokenizer tknzr;
    tknzr.TrimChars= A_CHAR( "\r" );
    String1K buf;
    buf.DbgDisableBufferReplacementWarning();
    SPFormatter formatter= Formatter::AcquireDefault(ALIB_CALLER_PRUNED);
    size_t entryNo= 1;
    for ( auto entry= begin(); entry != end(); ++entry )
    {
        text.Add( A_CHAR("{}{}: {!Q<>}"), (entry->Type.Integral() >= 0 ? 'E' : 'I'), entryNo, entry->Type );
        text.PushIndent( A_CHAR("    ") );
        try
        {
            formatter->FormatArgs( buf.Reset(), *entry );
        }
        catch( Exception& e )
        {
            buf <<  lib::RESULTS.GetResource("ExcFmtExc");
            e.Format( buf );
        }
        tknzr.Set( buf, '\n' );
        while( tknzr.HasNext() )
            text.Add( tknzr.Next()  );

        text.PopIndent();
        ++entryNo;
    }

    formatter->Release();
    return target;
}

}}} // namespace [aworx::lib::results]

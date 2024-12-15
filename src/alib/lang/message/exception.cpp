// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/lang/message/exception.hpp"
#   include "alib/strings/util/tokenizer.hpp"
#   include "alib/lang/format/formatterstdimpl.hpp"
#   include "alib/lang/format/paragraphs.hpp"
#   include "alib/lang/message/report.hpp"
#   include "alib/lang/message/exception.hpp"
#   include "alib/enums/recordparser.hpp"
#   include "alib/enums/detail/enumrecordmap.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"
#   include <algorithm>
#endif // !DOXYGEN


ALIB_BOXING_VTABLE_DEFINE( alib::lang::Exception*, vt_alib_exception )


namespace alib::lang {

void ERException::Parse()
{
    enums::EnumRecordParser::Get( ERSerializable::EnumElementName       );
                                  ERSerializable::MinimumRecognitionLength= 0;
    enums::EnumRecordParser::Get( DescriptionOrItsResourceName   , true );
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
            auto* tryRecord= message->Type.TryRecord<ERException>();
            if( tryRecord == nullptr )
            {
                std::vector<std::pair<integer,const void*>> recordList;
                for( auto& record : enums::detail::getInternalRecordMap() )
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
                    ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
                    Formatter& formatter= *Formatter::Default;
                    formatter.GetArgContainer();
                    formatter.Format( recordListDump,
                                      "Enum record {} not found for exception enumeration type {}.\n"
                                      "The following records have been found:\n",
                                      message->Type.Integral(),
                                      message->Type.TypeID()     );

                    for( auto& pair : recordList )
                        formatter.Format( recordListDump, "  {:2}: {}\n",
                                          pair.first,
                                          reinterpret_cast<const ERException*>( pair.second )->EnumElementName );
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
            buf <<  BASECAMP.GetResource("ExcFmtExc");
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

} // namespace [alib::lang]

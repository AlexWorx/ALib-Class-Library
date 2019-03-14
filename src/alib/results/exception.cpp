// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_RESULTS_EXCEPTION)
#include "alib/results/exception.hpp"
#endif

#if !defined (HPP_ALIB_STRINGFORMAT_FORMATTER_STD)
#   include "alib/stringformat/formatterstdimpl.hpp"
#endif

#if !defined (HPP_ALIB_STRINGFORMAT_TEXT)
#   include "alib/stringformat/text.hpp"
#endif

#if !defined (HPP_ALIB_RESULTS_EXCEPTION)
#   include "alib/results/exception.hpp"
#endif

DOX_MARKER([DOX_ALIB_ENUM_META_DATA_SPECIFCATION_impl])
#if !defined (HPP_ALIB_RESOURCE_TUPLE_LOADER)
#   include "alib/resources/resourcedtupleloader.hpp"
#endif

ALIB_ENUM_SPECIFICATION_IMPL( aworx::lib::results::Exception )
DOX_MARKER([DOX_ALIB_ENUM_META_DATA_SPECIFCATION_impl])

namespace aworx { namespace lib { namespace results {

void Exception::construct()
{
    MemoryBlocks* memory= MemoryBlocks::Create( 512 );
    instance=  memory->Alloc<This>();
    instance->memory    = memory;
    instance->firstEntry= nullptr;
}

Message* Exception::allocMessage()
{
    // find pointer to the last entry pointer;
    MessageEntry** tail= &instance->firstEntry;
    while(*tail != nullptr)
        tail= &(*tail)->next;

    *tail= instance->memory->Alloc<MessageEntry>();
    (*tail)->next= nullptr;


    return & (*tail)->message;
}

Exception::~Exception()
{
    if( instance )
    {
        auto* entry= instance->firstEntry;
        while( entry )
        {
            entry->message.~Message();
            entry= entry->next;
        }

        // as the memory object was allocated "in itself", we just need to clear it.
        instance->memory->~MemoryBlocks();
        instance= nullptr;
    }
}

Message&   Exception::Back() const
{
    auto* result= instance->firstEntry;
    while( result->next != nullptr )
        result= result->next;

    return result->message;
}

int   Exception::Size() const
{
    int result= 1;
    auto* entry= instance->firstEntry;
    while( entry->next != nullptr )
    {
        entry= entry->next;
        result++;
    }

    return result;
}

const Enum&   Exception::Type() const
{
    auto* entry= instance->firstEntry;
    Enum& result= entry->message.Type;
    while( (entry= entry->next) != nullptr )
        if( entry->message.Type.Value() >= 0 )
            result= entry->message.Type;

    return result;
}



void   Exception::Format( AString& target ) const
{
    Text text(target);
    Tokenizer tknzr;
    tknzr.TrimChars= A_CHAR( "\r" );
    String256 buf;
    buf.DbgDisableBufferReplacementWarning();
    SPFormatter formatter= GetDefaultFormatter();

    size_t entryNo= 1;
    for ( auto entry= begin(); entry != end(); entry++ )
    {
        text.Add( A_CHAR("E{}: <{}>"), entryNo, entry->Type );
        text.PushIndent( A_CHAR("    ") );
        formatter->FormatArgs( buf.Reset(), entry->Args );
        tknzr.Set( buf, '\n' );
        while( tknzr.HasNext() )
            text.Add( tknzr.Next()  );

        text.PopIndent();
        entryNo++;
    }
}



}}} // namespace [aworx::lib::results]

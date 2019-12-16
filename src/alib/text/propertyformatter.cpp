// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_TEXT_PROPERTY_FORMATTER)
#      include "alib/text/propertyformatter.hpp"
#   endif
#   if !defined(HPP_ALIB_TEXT_TEXT)
#      include "alib/text/text.hpp"
#   endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib { namespace text {

PropertyFormatter::PropertyFormatter( const String             customFormatString,
                                      const TCallbackTable&    propertyTable,
                                      SPFormatter              formatter,
                                      character                ESCCharacter          )
: stdFormatter        ( formatter )
, propertyFormatString( customFormatString )
, formatString        ( customFormatString )
{
    if(!formatter.get())
        stdFormatter= Formatter::GetDefault();

    integer parsePos= 0;
    while(parsePos < formatString.Length() )
    {
        // find next parse position
        integer parsePosCopy= parsePos;
        if( (parsePos= formatString.IndexOf( ESCCharacter, parsePosCopy ) ) < 0 )
            break;

        String identifier;
        integer endPos= parsePos+ 1;
        if( endPos < formatString.Length() )
        {
            // double Escape character? -> replace to single!
            if( formatString[endPos] == ESCCharacter )
            {
                formatString.Delete( endPos, 1 );
                ++parsePos;
                continue;
            }

            // find end of Identifier
            while( endPos < formatString.Length()  && isalpha( formatString[endPos] ) )
                ++endPos;

            identifier= formatString.Substring<false>( parsePos + 1, endPos - parsePos - 1 );
        }

        // not found
        if( identifier.IsEmpty() )
            continue;

        // add callback
        auto entryIt= propertyTable.begin();
        while( entryIt != propertyTable.end() )
        {
            if( Substring(identifier).ConsumePartOf<Case::Ignore>( entryIt->Name,
                                                                   entryIt->MinimumRecognitionLength
                                                                 ) == identifier.Length() )
            {
                // remove identifier from format string
                formatString.Delete( parsePos, endPos - parsePos + (formatString[endPos] == ESCCharacter ? 1 : 0) );

                // store callback
                callBacks.emplace_back( &*entryIt );
                ++parsePos;
                break;
            }
            ++entryIt;
        }

        // identifier not found?
        if( entryIt == propertyTable.end() )
        {
            Exception e( ALIB_CALLER_NULLED, Exceptions::UnknownPropertyInFormatString,
                         ESCCharacter, identifier, customFormatString  );
            for( auto& row : propertyTable )
               e.Back().Add( ESCCharacter, row.Name, ", " );
            e.Back().back()= '.'; // replace the last comma

            throw e;
        }
    }
}


void    PropertyFormatter::Format( AString& target, const Box&  src )
{
    // string buffers
    std::vector<String> heapStrings;
    String128   localString;
    localString.DbgDisableBufferReplacementWarning();

    // collect boxes
    Boxes& results= stdFormatter->Acquire(ALIB_CALLER_PRUNED);
    results.Add( formatString );
    for( auto& entry : callBacks )
    {
        Box argument= entry->Callback( src, localString );
        if( localString.IsNotEmpty() )
        {
            heapStrings.emplace_back( localString );
            argument= heapStrings.back();
        }

        results.Add ( argument  );
        localString.Reset();
    }

    // invoke ALib default formatter
    try
    {
        stdFormatter->FormatArgs( target, results );
    }
    catch(Exception& e)
    {
        e.Add( ALIB_CALLER_NULLED, Exceptions::ErrorInResultingFormatString, propertyFormatString );
        throw;
    }

    stdFormatter->Release();
    results.clear();
}


}}} // namespace [aworx::lib::text]

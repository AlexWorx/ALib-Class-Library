// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/stringformat/propertyformatter.hpp"

#if !defined (HPP_ALIB_STRINGS_UTIL_STRINGTUPLES)
#include "alib/strings/util/stringtuples.hpp"
#endif

namespace aworx { namespace lib { namespace stringformat {


PropertyFormatter::PropertyFormatter( const String             customFormatString,
                                      const TCallbackTable&    propertyTable,
                                      SPFormatter              formatter,
                                      character                ESCCharacter          )
: stdFormatter        ( formatter )
, propertyFormatString( customFormatString )
, formatString        ( customFormatString )
{
    if(!formatter.get())
        stdFormatter= STRINGFORMAT.GetDefaultFormatter();

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
                parsePos++;
                continue;
            }

            // find end of Identifier
            while( endPos < formatString.Length()  && isalpha( formatString[endPos] ) )
                endPos++;

            identifier= formatString.Substring<false>( parsePos + 1, endPos - parsePos - 1 );
        }

        // found
        if( !identifier.IsEmpty() )
        {
            // add callback
            auto callBack= lib::strings::util::FindStringStartInTupleVector<Case::Ignore, 0, 1>
                                                      ( propertyTable, identifier  );
            if( callBack == nullptr )
            {
                 Exception e( ALIB_CALLER_NULLED, Exceptions::UnknownPropertyInFormatString,
                              ESCCharacter, identifier, customFormatString  );
                 Boxes& args= e.Back().Args;
                 for( auto row : propertyTable )
                 {
                    args.Add( ESCCharacter );
                    args.Add( std::get<0>( row ) );
                    args.Add( ", " );
                 }
                 args.back()= '.'; // replace the last comma

                 throw e;
            }
            // remove identifier from format string
            formatString.Delete( parsePos, endPos - parsePos + (formatString[endPos] == ESCCharacter ? 1 : 0) );

            // store callback
            callBacks.emplace_back( callBack );
        }

        parsePos++;
    }
}


void    PropertyFormatter::Format( AString& target, const Box&  src )
{
    // string buffers
    std::vector<String> heapStrings;
    String128   localString;
    localString.DbgDisableBufferReplacementWarning();

    // collect boxes
    results.reserve( callBacks.size() + 1 );
    results.Add( formatString );
    for( auto entry : callBacks )
    {
        Box argument= std::get<2>(*entry)( src, localString );
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
        e.Add( ALIB_CALLER_NULLED, Exceptions::ErrorInResultingFormatString,
               propertyFormatString );
        throw;
    }

    results.clear();
}


}}} // namespace [aworx::lib::stringformat]

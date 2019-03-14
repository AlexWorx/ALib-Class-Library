// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_STRINGS_UTIL_AUTOSIZES)
#include "alib/strings/util/autosizes.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
    #include "alib/strings/localstring.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_CSTRING)
#   include "alib/strings/cstring.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
#   include "alib/strings/substring.hpp"
#endif


namespace aworx { namespace lib { namespace strings { namespace util  {

integer   AutoSizes::Actual( Types type, integer requestedSize, integer growthPadding )
{
    // grow arrays as needed
    while ( data.size() <= ActualIndex )
        data.emplace_back( type, -1, -1 );

    data[ActualIndex].type= type;

    // set measured size as it would be for the next session
    integer session=    data[ ActualIndex ].session;
    if ( session < requestedSize )
    {
        data[ ActualIndex ].session =  requestedSize;
    }

    // get size as it is for actual values (the ones that might have been imported)
    integer actual=       data[ ActualIndex ].actual;
    if ( actual <  requestedSize )
    {
        actual=  data[ ActualIndex ].actual=  (requestedSize + ( actual    < 0 ? 0 : growthPadding ));
    }

    return static_cast<int>(actual);
}


void    AutoSizes::Export( AString& target  )
{
    for( auto& entry : data )
    {
        target._( '(' ) ._( entry.type == Types::Tabstop ? 'T' : 'F')._( ',' )
                        ._( entry.actual                            )._( ',' )
                        ._( entry.session                           )           ._( ')' );
    }

    // remove unused entries at the end
    while( target.EndsWith(A_CHAR(",0,0)") ) )
        target.DeleteEnd<false>(target.Length() - target.LastIndexOf('('));
}

void    AutoSizes::Import( const String& sourceString, CurrentData session  )
{
    Reset();

    #if ALIB_DEBUG
    #   define PARSERROR ALIB_WARNING(                                                             \
            NString512("Error reading tab stops string \"") << NString512(sourceString)            \
                    << "\":\\n   at position " << (sourceString.Length() - parser.Length())    )
    #else
    #   define PARSERROR
    #endif
    Substring parser(sourceString);
    while(parser.Trim().IsNotEmpty())
    {
        parser.TrimStart();
        Types type;
        if( !parser.ConsumeChar<Case::Sensitive, Whitespaces::Trim>( '(' ) )
        {
            PARSERROR
            break;
        }

        if( parser.ConsumeChar<Case::Ignore, Whitespaces::Trim>( 'T' ) )
            type= Types::Tabstop;
        else if( parser.ConsumeChar<Case::Ignore,Whitespaces::Trim>( 'F' ) )
            type= Types::Field;
        else
        {
            PARSERROR
            break;
        }

        if( !parser.ConsumeChar<Case::Sensitive, Whitespaces::Trim>( ',' ) )
        {
            PARSERROR
            break;
        }


        integer   actual;
        parser.ConsumeInt( actual        );

        if( !parser.ConsumeChar<Case::Sensitive, Whitespaces::Trim>( ',' ) )
        {
            PARSERROR
            break;
        }


        integer   sessionValue;
        parser.ConsumeInt( sessionValue );

        if( !parser.ConsumeChar<Case::Sensitive, Whitespaces::Trim>( ')' ) )
        {
            PARSERROR
            break;
        }

        data.emplace_back( type, actual, sessionValue );
    }

    if( session == CurrentData::Clear )
        Consolidate();
}

void    AutoSizes::Consolidate()
{
    integer tabDiff     = 0;
    integer lastTabStop = 0;
    for( auto& entry : data )
    {
        integer actDiff=  entry.session - entry.actual;
        if( actDiff > 0 )  // should never happen. Maybe improper data import?
            actDiff= 0;
        if( entry.type == Types::Tabstop )
        {
            // reset tab difference if (for some strange application specific reason) this
            // tab stop is smaller than the previous one. Obviously some multi-line tab stop
            // is used (does not happen for example with with ALox)
            if( entry.actual > lastTabStop )
            {
                lastTabStop= entry.actual;
                entry.actual= entry.session + tabDiff;
            }
            else
            {
                lastTabStop= entry.actual;
                tabDiff= 0;
                entry.actual= entry.session + tabDiff;
            }
        }
        else
        {
            entry.actual     = entry.session;
        }
        tabDiff+= actDiff;
        entry.session  = -1;
    }
}


}}}} // namespace [aworx::lib::strings::util]

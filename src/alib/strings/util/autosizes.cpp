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
#include "alib/strings/strings.prepro.hpp"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Strings.AutoSizes;
    import ALib.Strings.Tokenizer;
#else
#   include "ALib.Strings.AutoSizes.H"
#   include "ALib.Strings.Tokenizer.H"
#endif
// ======================================   Implementation   =======================================
namespace alib {  namespace strings { namespace util  {

integer   AutoSizes::Actual( Types type, integer requestedSize, integer growthPadding )
{
    // grow arrays as needed
    while ( data.size() <= ActualIndex )
    {
        data.emplace_back( type, -1, -1 );
        dirty= true;
    }

    if ( data[ActualIndex].type != type )
    {
        data[ActualIndex].type  = type;
        data[ActualIndex].actual=  0;
        data[ActualIndex].session= -1;
        dirty= true;
    }
    
    if( WriteProtected )
        return (std::max)( data[ ActualIndex ].actual, requestedSize );

    // set measured size as it would be for the next session
    integer& session=   data[ ActualIndex ].session;
    if ( session < requestedSize )
    {
        session=  requestedSize;
        dirty= true;
    }

    // get size as it is for actual values (max of imported and session)
    integer& actual=    data[ ActualIndex ].actual;
    if ( actual <  requestedSize )
    {
        actual=  (requestedSize + ( actual    < 0 ? 0 : growthPadding ));
        dirty= true;
    }

    return actual;
}

void    AutoSizes::Export( AString& target  )
{
    if( WriteProtected )
        target._( "! ");
    
    auto it= data.begin();
    while( it!=data.end() )
    {
        target <<  ((*it).type == Types::Tabstop ? 'T' : 'F'     )
               <<  (*it).actual;
        if( !WriteProtected && (*it).session != (*it).actual)
            target << ',' << (*it).session;

        ++it;
        if( it!=data.end())
            target << '/';
        else
            break;
  }

    // remove unused entries at the end
    while(     target.EndsWith(A_CHAR("/T0"  ))
           ||  target.EndsWith(A_CHAR("/F0"  ))
           ||  target.EndsWith(A_CHAR("/T-1" ))
           ||  target.EndsWith(A_CHAR("/F-1" ))      )
        target.DeleteEnd<NC>(target.Length() - target.LastIndexOf('/'));

    dirty= false;
}

void    AutoSizes::Import( const String& src, lang::CurrentData session  )
{
    Reset();
    dirty= false;

    #if ALIB_DEBUG
    #   define PARSERROR    ALIB_WARNING( "STRINGS",                                               \
            "Error reading tab stops string \"{}\":\n   at position ",                             \
            src, src.Length() - parser.Length()       )
    #else
    #   define PARSERROR
    #endif
    Substring parser(src);
    WriteProtected= parser.ConsumeChar<lang::Case::Sensitive, lang::Whitespaces::Trim>('!');
    if(parser.Trim().IsEmpty())
        return;

    Tokenizer tknzr( parser, '/' );
    while(tknzr.HasNext())
    {
        parser= tknzr.Next();
        Types type;
             if( parser.ConsumeChar<lang::Case::Ignore, lang::Whitespaces::Trim>( 'T' ) )  type= Types::Tabstop;
        else if( parser.ConsumeChar<lang::Case::Ignore, lang::Whitespaces::Trim>( 'F' ) )  type= Types::Field;
        else { PARSERROR break; }

        integer   actual;
        parser.ConsumeInt( actual        );

        integer   sessionValue;
        if( !parser.ConsumeChar<lang::Case::Sensitive, lang::Whitespaces::Trim>( ',' ) )
            sessionValue= actual;
        else
            parser.ConsumeInt( sessionValue );

        data.emplace_back( type, actual, sessionValue );
    }

    if( session == lang::CurrentData::Clear )
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
            // reset tab difference if (for some strange application-specific reason) this
            // tab stop is smaller than the previous one. Obviously some multi-line tab stop
            // is used (does not happen for example with ALox)
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


}}} // namespace [alib::strings::util]


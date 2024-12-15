// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/lang/format/fmtcallerinfo.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"


#   include "alib/lang/basecamp/basecamp.hpp"
#   include "alib/strings/numberformat.hpp"
#   include "alib/strings/substring.hpp"
#   include "alib/lang/resources/resources.hpp"
#   include "alib/enums/recordbootstrap.hpp"
#   include "alib/enums/serialization.hpp"
#   include "alib/lang/format/fwds.hpp"
#   if ALIB_THREADS
#       include "alib/threads/thread.hpp"
#   endif
#endif // !DOXYGEN

namespace   alib::lang::format {

void FFormat_CallerInfo( const Box& box, const String& formatSpec, NumberFormat&, AString& target )
{
    FMTCallerInfo fmtCI{*box.Unbox<CallerInfo*>() };
    fmtCI.Format( formatSpec.IsNotEmpty() ? formatSpec
                                          : BASECAMP.GetResource("FMTCI"),
                target );
}

AString& FMTCallerInfo::Format( Substring format, AString& target, lang::CurrentData targetData ) const
{
    if ( targetData == lang::CurrentData::Clear )
        target.Reset();

    // this ensures that target is not nulled, as all other appends are NC-versions
    target._("");

    while ( format.IsNotEmpty() )
    {
        // read n equal characters
        int   n=  1;
        character c=  format.ConsumeChar();
        while ( format.ConsumeChar(c) )
            ++n;

        switch (c)
        {
            case '\'': // single quotes
            {
                // one or more pairs of single quotes?
                if ( n > 1 )
                {
                    int pairs= n / 2;
                    target.InsertChars<NC>( '\'', pairs );
                    n-= (pairs * 2);
                }

                // one single quote?
                if ( n == 1 )
                {
                    // search end
                    integer end= format.IndexOf( '\'' );
                    if ( end < 1 )
                    {
                        ALIB_WARNING( "ALIB", "Format Error: Missing single Quote" )
                        target <<     "Format Error: Missing closing single quote character <'>" ;
                        return target;
                    }

                    target._<NC>( format, 0, end );
                    format.ConsumeChars<NC>( end + 1 );
                }

            } break;

            // source information
            case 's':
            {
                if ( n == 1 )
                {
                    if ( format.ConsumeChar('f') )
                    {
                        if( ci.File ) target._<NC>( ci.File );
                        else          target._<NC>( "<NULL>" );
                        break;
                    }
                    if ( format.ConsumeChar('l') )
                    {
                        if( ci.File )
                            target._<NC>( ci.Line );
                        break;
                    }
                    if ( format.ConsumeChar('m') )
                    {
                        if( ci.File )
                            target._<NC>( ci.Func );
                        break;
                    }
                }
                // otherwise: copy what was in
                target.InsertChars<NC>( c, n );
                break;
            }

        #if ALIB_THREADS
            // thread information
            case 't':
            {
                if ( n == 1 )
                {
                    Thread* thread= Thread::Get(ci.ThreadID);

                    // alib thread name
                    if ( format.ConsumeChar('n') )
                    {
                        target._<NC>( thread ? thread->GetName()
                                                : BASECAMP.GetResource("FMTCINT") );
                        break;
                    }
                    // alib thread id
                    if ( format.ConsumeChar('i') )
                    {
                        if ( thread ) target._<NC>( thread->GetID() );
                        else          target._<NC>( BASECAMP.GetResource("FMTCINR")  );
                        break;
                    }

                    // native thread id
                    if ( format.ConsumeChar('c') )
                    {
                        if constexpr ( sizeof(std::thread::id) == sizeof(uint16_t) )
                        {
                            uint16_t nativeID= 0;
                            memcpy(&nativeID, &ci.ThreadID, 2);
                            target._<NC>("0x")._<NC>(Format::Hex(nativeID, 4));
                        }
                        if constexpr ( sizeof(std::thread::id) == sizeof(uint32_t) )
                        {
                            uint32_t nativeID= 0;
                            memcpy(&nativeID, &ci.ThreadID, 4);
                            target._<NC>("0x")._<NC>(Format::Hex(nativeID, 8));
                        }
                        if constexpr ( sizeof(std::thread::id) == sizeof(uint64_t) )
                        {
                            uint64_t nativeID= 0;
                            memcpy(&nativeID, &ci.ThreadID, 8);
                            target._<NC>("0x")._<NC>(Format::Hex(nativeID, 16));
                        }

                        break;
                    }

                    // all/auto
                    if ( format.ConsumeChar('a') )
                    {
                        target._<NC>( ci.ThreadID );
                        break;
                    }
                }
                // otherwise: copy what was in
                target.InsertChars<NC>( c, n );
                break;
            }
        #endif // ALIB_THREADS

            // type information
            case 'y':
            {
                if ( n == 1 )
                {
                    // full type name
                    if ( format.ConsumeChar('f') )
                    {
                      #if !ALIB_DEBUG
                        target._<NC>( BASECAMP.GetResource("FMTCINY") );
                      #else
                        if (!ci.TypeInfo) target._<NC>( BASECAMP.GetResource("FMTCINY") );
                        else              target << lang::DbgTypeDemangler(*ci.TypeInfo).Get();
                      #endif
                        break;
                    }

                    // stripped type name
                    if ( format.ConsumeChar('n') )
                    {
                      #if !ALIB_DEBUG
                        target._<NC>( BASECAMP.GetResource("FMTCINY") );
                      #else
                        if (!ci.TypeInfo)
                            target._<NC>( BASECAMP.GetResource("FMTCINY") );
                        else
                        {
                            NString2K typeName;
                            DbgTypeDemangler(*ci.TypeInfo).GetShort(typeName);
                            target._<NC>( typeName );
                        }
                      #endif
                        break;
                    }

                    // automatic type together with shortened function name
                    if ( format.ConsumeChar('a') )
                    {
                      #if !ALIB_DEBUG
                        target._<NC>( ci.Func )._<NC>("()");
                      #else
                        if (ci.TypeInfo)
                            target._<NC>( *ci.TypeInfo )._<NC>("::");
                        target._<NC>( ci.Func )._<NC>("()");
                      #endif
                        break;
                    }

                }
                // otherwise: copy what was in
                target.InsertChars<NC>( c, n );
                break;
            }

            default: // otherwise: copy what was in
                target.InsertChars<NC>( c, n );
                break;
        }

    }

    return target;
}


} // namespace [  alib::lang::system]


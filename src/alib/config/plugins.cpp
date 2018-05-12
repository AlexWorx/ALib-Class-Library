// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
#   include "alib/config/configuration.hpp"
#endif
#if !defined (HPP_ALIB_SYSTEM_ENVIRONMENT)
#   include "alib/system/environment.hpp"
#endif


#if !defined(_STDIO_H) && !defined(_INC_STDIO)
    #include <stdio.h>  /* defines FILENAME_MAX */
#endif

namespace aworx { namespace lib { namespace config {

// #################################################################################################
// XTernalizer
// #################################################################################################
void XTernalizer::InternalizeValue( Substring& src, AString& dest )
{
    src.Trim();
    if(src.CharAtStart() == '"'  && src.CharAtEnd() == '"')
    {
        src.ConsumeChar       <false>();
        src.ConsumeCharFromEnd<false>();
    }
    bool lastWasSlash= false;

    while( src.IsNotEmpty() )
    {
        character c= src.ConsumeChar<false>();

        if( lastWasSlash )
        {
            lastWasSlash= false;
            character escChr= c == '\\' ? '\\' :
                          c == '"'  ? '"'  :
                          c == 'n'  ? '\n' :
                          c == 'r'  ? '\r' :
                          c == 't'  ? '\t' :
                          c == 'a'  ? '\a' :
                          c == 'b'  ? '\b' :
                          c == 'v'  ? '\v' :
                          c == 'f'  ? '\f' :
                          c == 'e'  ? '\033' :
                          c;

            dest._<false>(escChr);
            continue;
        }

        if( c== '\\' )
        {
            lastWasSlash= true;
            continue;
        }

        dest._<false>(c);
    }
}

void XTernalizer::ExternalizeValue( Substring& src, AString& dest, character delim )
{
    bool needsQuotes=       src.CharAtStart() == ' '
                        ||  src.CharAtStart() == '\t'
                        ||  src.CharAtEnd()   == ' '
                        ||  src.CharAtEnd()   == '\t'
                        ||  src.IndexOf( delim ) >= 0;
    if ( needsQuotes )
        dest._<false>('"');

    while( src.IsNotEmpty() )
    {
        character c= src.ConsumeChar();

        switch(c)
        {
            case '"'    : dest._<false>(needsQuotes ? "\\\"" : "\"");
                          break;
            case '\\'   : dest._<false>("\\\\"); break;
            case '\r'   : dest._<false>("\\r" ); break;
            case '\n'   : dest._<false>("\\n" ); break;
            case '\t'   : dest._<false>("\\t" ); break;
            case '\a'   : dest._<false>("\\a" ); break;
            case '\b'   : dest._<false>("\\b" ); break;
            case '\v'   : dest._<false>("\\v" ); break;
            case '\f'   : dest._<false>("\\f" ); break;
            case '\033' : dest._<false>("\\e" ); break;
            default     : dest._<false>(c);      break;
        }
    }

    if ( needsQuotes )
        dest._('"');
}

void XTernalizer::LoadFromString( Variable& variable, const String& value )
{
    variable.ClearValues();
    AString* varValue= &variable.Add();
    Substring src( value );

    if( variable.Delim == '\0' )
    {
        InternalizeValue( src, *varValue );
        return;
    }

    // tokenize
    bool inQuote=      false;
    bool lastWasSlash= false;
    integer idx=          0;
    while( idx < src.Length()  )
    {
        character c= src.CharAt<false>( idx++ );

        if( lastWasSlash )
        {
            lastWasSlash= false;
            continue;
        }

        if( c== '\\' )
        {
            lastWasSlash= true;
            continue;
        }

        if( c== '"' )
        {
            inQuote= !inQuote;
            continue;
        }

        if( !inQuote && c == variable.Delim )
        {
            Substring tok= src.Substring<false>( 0, idx - 1 );
            InternalizeValue( tok, *varValue );
            varValue= &variable.Add();
            src.ConsumeChars( idx );
            src.TrimStart();
            idx= 0;
        }
    }

    if ( src.IsNotEmpty() )
    {
        InternalizeValue( src, *varValue );
    }
}

// #################################################################################################
// CLIArgs
// #################################################################################################
bool  CLIArgs::Load( Variable& variable, bool searchOnly )  const
{
    // check if category may me left out
    bool allowWithoutCategory= false;
    for (auto& defaultCategory : DefaultCategories )
        if( (allowWithoutCategory= variable.Category.Equals( defaultCategory )) == true )
            break;

    String256   stringConverter;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( stringConverter,  ReplaceExternalBuffer )

    size_t qtyArgs= args != nullptr ?  args->size()
                                    :  argCount;
    for ( size_t i= args != nullptr ? 0 : 1 ; i < qtyArgs ; i++ )
    {
        // create substring on actual variable (trim if somebody would work with quotation marks...)
        Substring cliArg;

        if( args != nullptr )
            cliArg= (*args)[i];
        else
        {
            if (!wArgs)
            {
                if(!std::is_same<character, char>::value)
                {
                    stringConverter.Clear()._( reinterpret_cast<char**>(argVector)[i] );
                    cliArg= stringConverter;
                }
                else
                    cliArg= reinterpret_cast<character**>(argVector)[i];
            }
            else
            {
                if(!std::is_same<character, wchar_t>::value)
                {
                    stringConverter.Clear()._( reinterpret_cast<wchar_t**>(argVector)[i] );
                    cliArg= stringConverter;
                }
                else
                    cliArg= reinterpret_cast<character**>(argVector)[i];
            }
        }

        cliArg.Trim();

        // request '-' and allow a second '-'
        if ( !cliArg.ConsumeChar('-') )
            continue;
        cliArg.ConsumeChar('-');

        // try names

        if (    !                          cliArg.ConsumeString<lang::Case::Ignore>( variable.Fullname )
             && !( allowWithoutCategory && cliArg.ConsumeString<lang::Case::Ignore>( variable.Name     )  )
             && !(    AllowedMinimumShortCut > 0
                   && (                            cliArg.ConsumePartOf( variable.Fullname, AllowedMinimumShortCut + 1 + static_cast<int>(variable.Category.Length()) )
                       ||( allowWithoutCategory && cliArg.ConsumePartOf( variable.Name    , AllowedMinimumShortCut ) )
                      )
                 )
           )
            continue; // next arg

        // found --CAT_NAME. If rest is empty or continues with '=', we set
        if ( cliArg.IsEmpty() )
        {
            if ( !searchOnly )
                variable.Add();
            return true;
        }

        if ( cliArg.ConsumeChar<true, Whitespaces::Trim>() == '='  )
        {
            if ( !searchOnly )
                StringConverter->LoadFromString( variable, cliArg.Trim() );
            return true;
        }
    }

    return false;
}

// #################################################################################################
// CLIArgs Iterator
// #################################################################################################
//! @cond NO_DOX
namespace {

class CLIArgsIteratorImpl : public ConfigurationPlugin::Iterator
{
    CLIArgs&        parent;
    String          sectionName;
    size_t          nextArgNo;


    public:

    CLIArgsIteratorImpl( CLIArgs& cliArgs, const String& pSectionName )
    : parent(cliArgs)
    , sectionName(pSectionName)
    , nextArgNo(0)
    {

    }

    virtual ~CLIArgsIteratorImpl() override
    {}

    virtual bool        Next( Variable& variable )                                          override
    {
        return detail::nextCLIArg( parent, nextArgNo, sectionName, variable );
    }
};

} // anonymous namespace

namespace detail {

bool nextCLIArg( CLIArgs& cliArgs, size_t& nextArgNo, const String& sectionName, Variable& variable )
{
    // clear variable name at least. Values remain, until something was found. The caller has
    // to check the result anyhow!
    variable.Name.Clear();

    size_t qtyArgs= cliArgs.args != nullptr ?  cliArgs.args->size()
                                            :  cliArgs.argCount;
    if( nextArgNo >= qtyArgs )
        return false;

    // check if category may me left out
    bool allowWithoutCategory= false;
    for (auto& defaultCategory : cliArgs.DefaultCategories )
        if( (allowWithoutCategory= sectionName.Equals( defaultCategory )) == true )
            break;

    String256   stringConverter;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( stringConverter,  ReplaceExternalBuffer )

    // skip index 0 (command name) if not special args string vector set.
    if( nextArgNo == 0 && cliArgs.args == nullptr )
        nextArgNo= 1;
    while( nextArgNo < qtyArgs )
    {
        // create substring on actual variable (trim if somebody would work with quotation marks...)
        Substring cliArg;
        if( cliArgs.args != nullptr )
            cliArg= (*cliArgs.args)[nextArgNo];
        else
        {
            if (!cliArgs.wArgs)
            {
                if(!std::is_same<character, char>::value)
                {
                    stringConverter.Clear()._( reinterpret_cast<char**>(cliArgs.argVector)[nextArgNo] );
                    cliArg= stringConverter;
                }
                else
                    cliArg= reinterpret_cast<character**>(cliArgs.argVector)[nextArgNo];
            }
            else
            {
                if(!std::is_same<character, wchar_t>::value)
                {
                    stringConverter.Clear()._( reinterpret_cast<wchar_t**>(cliArgs.argVector)[nextArgNo] );
                    cliArg= stringConverter;
                }
                else
                    cliArg= reinterpret_cast<character**>(cliArgs.argVector)[nextArgNo];
            }
        }
        cliArg.Trim();
        nextArgNo++;

        // request '-' and allow a second '-'
        if ( !cliArg.ConsumeChar('-') )
            continue;
        cliArg.ConsumeChar('-');

        // consume category
        if( !allowWithoutCategory && !sectionName.IsEmpty()
            && (    !cliArg.ConsumeString<lang::Case::Ignore>( sectionName )
                 || !cliArg.ConsumeChar('_' ) ) )
           continue;

        // check for '=' sign
        integer equalSignPos= cliArg.IndexOf( '=' );
        if( equalSignPos < 0 )
            continue;

        // found!
        Substring value;
        cliArg  .Split  ( equalSignPos, value, 1);
        variable.Declare( sectionName, cliArg   );
        cliArgs.StringConverter->LoadFromString( variable, value.Trim() );
        return true;
    }

    // not found
    return false;
}
} //namespace detail

ConfigurationPlugin::Iterator*   CLIArgs::GetIterator( const String& sectionName )
{
    return new CLIArgsIteratorImpl( *this, sectionName );
}

//! @endcond

// #################################################################################################
// Environment
// #################################################################################################
Environment::Environment()
: ConfigurationPlugin()
{
}

bool  Environment::Load( Variable& variable, bool searchOnly )  const
{
    String256 value;
    ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( value,  ReplaceExternalBuffer );
    system::GetEnvironmentVariable( variable.Fullname, value, lang::CurrentData::Keep );
    if ( value.IsEmpty() )
        return false;

    if( !searchOnly )
        StringConverter->LoadFromString( variable, value );
    return true;
}



}}}// namespace [aworx::lib::config]


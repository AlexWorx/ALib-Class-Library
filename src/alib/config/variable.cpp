// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif

#if !defined(HPP_ALIB_STRINGS_NUMBERFORMAT)
    #include "alib/strings/numberformat.hpp"
#endif

#if !defined(HPP_ALIB_STRINGS_UTIL_TOKENIZER)
    #include "alib/strings/util/tokenizer.hpp"
#endif

#if !defined (HPP_ALIB_LANG_RESOURCE_TUPLE_LOADER)
    #include "alib/lang/resourcedtupleloader.hpp"
#endif




namespace aworx { namespace lib { namespace config {

void    Variable::clear()
{
    Config=         nullptr;
    Priority=       Priorities::NONE;
    Delim=          '\0';
    FmtHints=       FormatHints::None;

    Category._();
    Name    ._();
    Comments._();
    Fullname._();
    ClearValues();

}

Variable&    Variable::Declare( const VariableDecl&  declaration,  Boxes& replacements )
{
    clear();

    Delim=               declaration.Delim();
    FmtHints=            declaration.FmtHints();
    FormatAttrAlignment= declaration.FormatAttrAlignment();

    // set Category, Name, Comment
    Category._( declaration.Category()  );
    Name    ._( declaration.Name()      );
    Comments._( declaration.Comments()  );

    if ( declaration.DefaultValue().IsNotNull() )
        DefaultValue._()._( declaration.DefaultValue() );
    else
        DefaultValue.SetNull();


    // replace strings
    int replCnt= 1;
    String128 replace;
    for ( auto& replacement : replacements )
    {
        String64  search("%"); search._( replCnt );
        replace.Clear() << replacement;
        if ( replacement.IsNotNull() ) { Category    .SearchAndReplace( search, replace );
                                         Name        .SearchAndReplace( search, replace );
                                         Comments    .SearchAndReplace( search, replace );
                                         DefaultValue.SearchAndReplace( search, replace );
                                       }

        replCnt++;
    }

    // set full name
    if( Category.IsNotEmpty() )
        Fullname._( Category )._( '_' );
    Fullname._( Name );

    ALIB_ASSERT_WARNING(  Name.IsNotEmpty(), ASTR("Empty variable name given") );

    return *this;
}

Variable&   Variable::Declare( const String& category,  const String& name,  character  delim ,
                               const String& comments     )
{
    clear();

    Category._(category);
    Name    ._(name);
    Comments._(comments);
    Delim=   delim;

    if( Category.IsNotEmpty() )
        Fullname._( Category )._( '_' );
    Fullname._( Name );

    return *this;
}

void Variable::ReplaceValue( int idx, Variable& replVariable )
{
    if( idx < 0 || idx >= qtyValues )
    {
        ALIB_WARNING( ASTR("Index out of range: "), idx  );
        return;
    }

    int replSize= replVariable.Size();
    if( replSize == 0 )
    {
        values.erase( values.begin() + idx );
        return;
    }

    values[static_cast<size_t>(idx)]._()._( replVariable.GetString(0) );
    for( int i= 1 ; i < replSize; i++ )
    {
        values.insert( values.begin() + idx + i, AString( replVariable.GetString(i) ) );
        qtyValues++;
    }
}

AString&    Variable::Add()
{
    size_t actIdx= static_cast<size_t>(qtyValues);
    qtyValues++;
    if( actIdx < values.size() )
        return values[actIdx].Clear();

    return *values.insert( values.end(), AString() );
}



bool        Variable::IsTrue    (int idx) { return  idx < qtyValues ? Config->IsTrue( GetString (idx) )                   : false; }
integer     Variable::GetInteger(int idx) { return  idx < qtyValues ? static_cast<integer>(GetString(idx)->ParseInt())    : 0;     }
double      Variable::GetFloat  (int idx) { return  idx < qtyValues ? GetString(idx)->ParseFloat( Config ? &Config->NumberFormat : &NumberFormat::Global ) : 0.0;   }


bool Variable::GetAttribute( const String& attrName, Substring& result, char attrDelim )
{
    for ( int i= 0; i< Size(); i++ )
    {
        result= GetString( i );
        if (    result.ConsumeString<lang::Case::Ignore, lang::Whitespaces::Trim>( attrName )
             && result.ConsumeChar<lang::Case::Ignore, lang::Whitespaces::Trim>( attrDelim ) )
        {
            result.Trim();
            return true;
        }
    }
    return false;
}


}}}// namespace [aworx::lib::config]


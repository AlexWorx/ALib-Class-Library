// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_CONFIG_VARIABLE)
    #include "alib/config/variable.hpp"
#endif

#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_RECORDPARSER)
#   include "alib/enums/recordparser.hpp"
#endif
#endif // !defined(ALIB_DOX)

namespace alib {  namespace config {

Variable&    Variable::Reset( lang::CurrentData nameAndCategory )
{
    auto& self= Self();
    String128 oldName;
    String128 oldCategory;
    if( nameAndCategory == lang::CurrentData::Keep )
    {
        oldName     << self.Name;
        oldCategory << self.Category;
    }

    SelfContained::Reset( &fields );
    self.Config       =  nullptr;
    self.Priority     =  Priorities::NONE;
    self.Delim        =  '\0';
    self.FmtHints     =  FormatHints::None;

    self.Category     =
    self.Name         =
    self.Comments     =
    self.Fullname     = EmptyString();
    self.DefaultValue = NullString();

    // our vector needs to be constructed in place! This is needed to make it "forget"
    // its internal data field.
    new (&self.values) std::vector<String,StdContMA<String>>( StdContMA<String>(Allocator()) );

    if( nameAndCategory == lang::CurrentData::Keep )
    {
        self.Name       = Allocator().EmplaceString( oldName     );
        self.Category   = Allocator().EmplaceString( oldCategory );
    }

    return *this;
}

Variable& Variable::Declare ( const Variable& variable )
{
    auto& self     = Self();
    auto& allocator= Allocator();

    Reset();

    self.Category= allocator.EmplaceString( variable.Category() );
    self.Name    = allocator.EmplaceString( variable.Name    () );
    self.Comments= allocator.EmplaceString( variable.Comments() );
    self.Delim   =                   variable.Delim   ()  ;
    self.Fullname= EmptyString();
    return *this;
}


Variable&    Variable::Declare( const VariableDecl&  declaration,  const Box& replacements )
{
    auto& self     = Self();
    auto& allocator= Allocator();
    Reset();

    self.Delim=                                 declaration.Delim;
    self.FmtHints=                              declaration.FmtHints;
    self.FormatAttrAlignment= allocator.EmplaceString(  declaration.FormatAttrAlignment );

    // set Category, Name, Comment
    String128 bufCategory;       ALIB_DBG( bufCategory    .DbgDisableBufferReplacementWarning() );
    String128 bufName;           ALIB_DBG( bufName        .DbgDisableBufferReplacementWarning() );
    String1K  bufComments;       ALIB_DBG( bufComments    .DbgDisableBufferReplacementWarning() );
    String128 bufDefaultValue;   ALIB_DBG( bufDefaultValue.DbgDisableBufferReplacementWarning() );

    bufCategory         << declaration.Category;
    bufName             << declaration.EnumElementName;
    bufComments         << declaration.Comments;
    if ( declaration.DefaultValue.IsNotNull() )
        bufDefaultValue << declaration.DefaultValue;

    // replace strings
    if( !replacements.IsType<void>() )
    {
        String128 replace;

        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        const Box*  replacementPtr;
        integer     qtyReplacements;
        if ( replacements.IsType<Boxes*>() )
        {
            const Boxes* boxes= replacements.Unbox<Boxes*>();
            replacementPtr = boxes->data();
            qtyReplacements= boxes->Size();
        }
        else if ( replacements.IsArrayOf<Box>() )
        {
            replacementPtr = replacements.UnboxArray<Box>();
            qtyReplacements= replacements.UnboxLength();
        }
        else
        {
            replacementPtr = &replacements;
            qtyReplacements= 1;
        }

        for ( integer  replCnt= 0; replCnt< qtyReplacements ; ++replCnt )
            if ( !replacementPtr->IsType<void>() )
            {
                String64  search("%"); search._( replCnt + 1 );
                replace.Reset( *( replacementPtr + replCnt) );
                bufCategory    .SearchAndReplace( search, replace );
                bufName        .SearchAndReplace( search, replace );
                bufComments    .SearchAndReplace( search, replace );
                bufDefaultValue.SearchAndReplace( search, replace );
            }
        ALIB_WARNINGS_RESTORE
    }

    ALIB_ASSERT_WARNING(  bufName.IsNotEmpty(), "CONFIG", "Empty variable name given" )

    self.Category           = allocator.EmplaceString( bufCategory     );
    self.Name               = allocator.EmplaceString( bufName         );
    self.Comments           = allocator.EmplaceString( bufComments     );

    if ( declaration.DefaultValue.IsNotNull() )
        self.DefaultValue   = allocator.EmplaceString( bufDefaultValue );
    else
        self.DefaultValue   = NullString();

    return *this;
}

Variable&   Variable::Declare( const String& category,  const String& name,  character  delim ,
                               const String& comments     )
{
    auto& self     = Self();
    auto& allocator= Allocator();

    Reset();

    self.Category         = allocator.EmplaceString( category );
    self.Name             = allocator.EmplaceString( name     );
    self.Comments         = allocator.EmplaceString( comments );
    self.Delim            = delim;

    return *this;
}

Variable&   Variable::Declare( const ComplementString& category,
                               const ComplementString& name,
                               complementChar          delim ,
                               const ComplementString& comments     )
{
    auto& self     = Self();
    auto& allocator= Allocator();

    String2K charWidthConverter;
    charWidthConverter.DbgDisableBufferReplacementWarning();

    Reset();

    self.Category         = allocator.EmplaceString( charWidthConverter.Reset(category)  );
    self.Name             = allocator.EmplaceString( charWidthConverter.Reset(name    )  );
    self.Comments         = allocator.EmplaceString( charWidthConverter.Reset(comments)  );
    self.Delim            = static_cast<character>( delim );

    return *this;
}

void Variable::ReplaceValue( int idx, const String& replacement )
{
    auto& self= Self();
    if( idx < 0 || idx >= Size() )
    {
        ALIB_WARNING( "CONFIG", "Index out of range: ", idx  )
        return;
    }

    self.values[static_cast<size_t>(idx)]= Allocator().EmplaceString( replacement );
}

void Variable::ReplaceValue( int idx, Variable& replVariable )
{
    auto& self     = Self();
    auto& allocator= Allocator();
    if( idx < 0 || idx >= Size() )
    {
        ALIB_WARNING( "CONFIG", "Index out of range: ", idx  )
        return;
    }

    integer replSize= replVariable.Size();
    if( replSize == 0 )
    {
        self.values.erase( self.values.begin() + idx );
        return;
    }
    self.values.reserve( static_cast<size_t>( Size() + replSize - 1 ) );
    self.values[static_cast<size_t>(idx)]= allocator.EmplaceString( replVariable.GetString() );
    for( int i= 1 ; i < replSize; ++i )
        self.values.insert( self.values.begin() + idx + i, allocator.EmplaceString( replVariable.GetString(i) ) );

}

void Variable::Add( const String& value )
{
    Self().values.push_back(Allocator().EmplaceString( value ) );
}


void Variable::Add( int64_t value )
{
    String128 converter(value);
    Self().values.push_back( Allocator().EmplaceString( converter ) );
}

void Variable::Add( double value )
{
    String128 converter(value);
    Self().values.push_back(  Allocator().EmplaceString( converter ) );
}


bool        Variable::IsTrue    (int idx) { return idx < Size() ? Self().Config->IsTrue( GetString (idx) )
                                                                : false; }
integer     Variable::GetInteger(int idx) { return idx < Size() ? static_cast<integer>( GetString(idx).ParseInt())
                                                                : 0;     }
double      Variable::GetFloat  (int idx) { return idx < Size() ? GetString(idx).ParseFloat( Self().Config ? &Self().Config->NumberFormat : &NumberFormat::Global )
                                                                : 0.0;   }


bool Variable::GetAttribute( const String& attrName, Substring& result, character attrDelim )
{
    for ( int i= 0; i< Size(); ++i )
    {
        result= GetString( i );
        if (    result.ConsumeString<lang::Case::Ignore, lang::Whitespaces::Trim>( attrName  )
             && result.ConsumeChar  <lang::Case::Ignore, lang::Whitespaces::Trim>( attrDelim ) )
        {
            result.Trim();
            return true;
        }
    }
    result= NullString();
    return false;
}

const String&  Variable::Fullname()
{
    auto& self     = Self();
    auto& allocator= Allocator();
    if( self.Fullname.IsEmpty() )
    {
        String256 bufFullname;   ALIB_DBG( bufFullname.DbgDisableBufferReplacementWarning() );
        if( self.Category.IsNotEmpty() )
            bufFullname <<  self.Category  << '_';
        bufFullname <<  self.Name;

        self.Fullname= allocator.EmplaceString( bufFullname );
    }

    return  self.Fullname;
}


DOX_MARKER([DOX_ALIB_ENUMS_RECORD_PARSER])
void VariableDecl::Parse()
{
    enums::EnumRecordParser::Get( Category                         );
    enums::EnumRecordParser::Get( ERSerializable::EnumElementName  );           // field from parent class
                                  ERSerializable::MinimumRecognitionLength= 0;  // omit reading, but fix to zero
    enums::EnumRecordParser::Get( Delim                     );
    enums::EnumRecordParser::Get( FormatAttrAlignment       );
    enums::EnumRecordParser::Get( FmtHints                  , true ); // last field!
}
DOX_MARKER([DOX_ALIB_ENUMS_RECORD_PARSER])

}} // namespace [alib::config]

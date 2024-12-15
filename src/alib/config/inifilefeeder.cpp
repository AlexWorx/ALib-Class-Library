// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/config/configcamp.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"
#   include "alib/config/inifilefeeder.hpp"
#   include "alib/config/configuration.hpp"
#   include "alib/lang/system/processinfo.hpp"
#   include "alib/strings/util/tokenizer.hpp"
#   include "alib/strings/util/tokenizer.hpp"
#   include "alib/lang/format/paragraphs.hpp"
#   include "alib/compatibility/std_strings_iostream.hpp"

#   include <fstream>
#   include <algorithm>
#endif // !DOXYGEN


namespace alib::config {


// #################################################################################################
// helpers
// #################################################################################################
std::pair<IniFile::Section*, IniFile::Entry*>   IniFileFeeder::SearchEntry ( const String& path )
{
    if(iniFile == nullptr)
    {
        ALIB_ERROR( "CONFIG", "No INI-file loaded when trying to search data." )
        return std::pair<IniFile::Section*, IniFile::Entry*>(nullptr, nullptr);
    }

    // separate section/entry name
    auto sectionSeparator= path.LastIndexOf( configuration.Separator() );
    String sectionName= (sectionSeparator != -1) ? path.Substring<NC>(0, sectionSeparator ) : EMPTY_STRING;
    String entryName  = (sectionSeparator != -1) ? path.Substring<NC>(sectionSeparator + 1, path.Length() - sectionSeparator - 1) : path;

    // search for existing entry
    return iniFile->SearchEntry(sectionName, entryName);
}

std::pair<IniFile::Section*, IniFile::Entry*>   IniFileFeeder::SearchEntry ( const Variable& var )
{
    ALIB_ASSERT_ERROR( var.IsDeclared(), "CONFIG", "Given Variable not declared." )
    ALIB_ASSERT_ERROR( &var.GetConfiguration() == &configuration, "CONFIG",
                        "Variable belongs to different configuration: ", String256(var))
    return SearchEntry( String256( var ) );
}

// #################################################################################################
// Import interface
// #################################################################################################
int     IniFileFeeder::ImportSection(  const String& sectionName, const String& typeName )
{
    if(iniFile == nullptr)
    {
        ALIB_ERROR( "CONFIG", "No INI-file loaded when trying to import data." )
        return 0;
    }

    auto* section= iniFile->SearchSection( sectionName );
    if(section == nullptr)
    {
        ALIB_WARNING( "CONFIG", "Section name not found in INI-file: ", sectionName )
        return 0;
    }

    int cnt= 0;
    String256 varName;

    varName.Reset(sectionName);
    if( varName.IsNotEmpty())
        varName << configuration.Separator();

    // loop over all entries
    for ( auto& entry : section->Entries )
    {
        StringLengthResetter sectionNameResetter(varName);
        varName << entry.Name;
        Variable var(configuration, varName, typeName);
        if( var.Define( priority) )
        {
            var.Import(entry.Value, priority, &configuration.Escaper );
            ++cnt;
        }
    }

    return cnt;
}

int     IniFileFeeder::importSection( IniFile::Section& section )
{
    int cnt= 0;
    String256 varName;

    varName.Reset(section.Name);
    if( varName.IsNotEmpty())
        varName << configuration.Separator();

    // loop over all entries
    for ( auto& entry : section.Entries )
    {
        // Try if variable is declared and has lower or equal priority than us.
        StringLengthResetter sectionNameResetter(varName);
        varName << entry.Name;
        Variable var(configuration);
        Substring value= entry.Value;
        if( var.Try(varName) )
        {
            var.Import(value, priority, &configuration.Escaper );
            ++cnt;
            continue;
        }

        // Variable not declared. Copy value to undeclared input variables
        configuration.PresetImportString(varName, value, &configuration.Escaper, priority);
    }

    return cnt;
}

int     IniFileFeeder::ImportSection(  const String& sectionName )
{
    if(iniFile == nullptr)
    {
        ALIB_ERROR( "CONFIG", "No INI-file loaded when trying to import data." )
        return 0;
    }

    auto* section= iniFile->SearchSection( sectionName );
    if(section == nullptr)
    {
        ALIB_WARNING( "CONFIG", "Section name not found in INI-file: ", sectionName )
        return 0;
    }

    return importSection(*section);
}

int     IniFileFeeder::ImportAll()
{
    if(iniFile == nullptr)
    {
        ALIB_ERROR( "CONFIG", "No INI-file loaded when trying to import data." )
        return 0;
    }

    int cnt= 0;
    String256 varName;

    // loop over all sections
    for ( IniFile::Section& section : iniFile->Sections )
        cnt+= importSection( section );

    return cnt;
}

// #################################################################################################
// Export interface
// #################################################################################################
bool         IniFileFeeder::Export( const Variable& var)
{
    if(iniFile == nullptr)
    {
        ALIB_ERROR( "CONFIG", "No INI-file loaded when trying to export data." )
        return false;
    }
    ALIB_ASSERT_ERROR(var.IsDeclared(), "CONFIG", "Variable to export not declared: ", String256(var))

    String256 name(var);

    // separate section/entry name
    auto sectionSeparator= name.LastIndexOf( var.GetConfiguration().Separator() );
    String sectionName= (sectionSeparator != -1) ? name.Substring<NC>(0, sectionSeparator ) : EMPTY_STRING;
    String entryName  = (sectionSeparator != -1) ? name.Substring<NC>(sectionSeparator + 1, name.Length() - sectionSeparator - 1) : name;

    // search for existing entry
    auto pair= iniFile->SearchEntry(sectionName, entryName);
    auto* entry= pair.second;
    if( entry )
    {
        // exists and no write back?
        if( !entry->WriteBack && !pair.first->WriteBack )
            return false;
    }
    else
    {
        // create entry
        auto sectionIt= iniFile->SearchOrCreateSection( sectionName );
        entry  = iniFile->CreateEntry( sectionIt.first, entryName );
    }
    
    String2K buf;
    var.Export( buf, &configuration.Escaper );
    entry->NewValue.Allocate(iniFile->Allocator, buf );

    // add comments
    if(entry->Comments.IsNull())
    {
        auto* decl= var.GetDeclaration();
        if( decl && decl->Comments().IsNotEmpty() )
            iniFile->AddComments( entry->Comments, decl->Comments(), DefaultCommentPrefix );
    }

    return true;
}

int  IniFileFeeder::ExportSubTree( Configuration::Cursor cursor, bool directChildrenOnly )
{
    if(iniFile == nullptr)
    {
        ALIB_ERROR( "CONFIG", "No INI-file loaded when trying to export data." )
        return 0;
    }
    int cnt= 0;
    // check cursor itself first
    if( !cursor.IsRoot() )
    {
        Variable var(cursor);
        if( var.IsDeclared() )
            if( Export( var ) )
                cnt++;
    }

    Configuration::RecursiveIterator rit;
    rit.SetPathGeneration( lang::Switch::On );
    rit.Initialize( cursor, directChildrenOnly ? 0 : (std::numeric_limits<unsigned int>::max)() );
    while ( rit.IsValid() )
    {
        if( rit.Node().Name().Equals(A_CHAR("$PRESETS")) )
        {
            rit.NextSibling();
            continue;
        }

        Variable var(rit.Node());
        if( var.IsDeclared() && var.IsDefined() )
        {
            if( Export( var ) )
                cnt++;
        }
        rit.Next();
    }
    return cnt;
}

int  IniFileFeeder::AddResourcedSectionComments( ResourcePool&    resourcePool,
                                                 const NString&   resourceCategory,
                                                 const NString&   resourceNamePrefix    )
{
    if(iniFile == nullptr)
    {
        ALIB_ERROR( "CONFIG", "No INI-file loaded when trying to import data." )
        return 0;
    }

    // add section comments from resources to INI-file
    int cnt= 0;
    for( auto& section : iniFile->Sections )
        if( section.Comments.IsNull() )
        {
            auto& comment=  resourcePool.Get( resourceCategory,
                                               NString128() << resourceNamePrefix << section.Name
                                               ALIB_DBG(, false));
            if( comment.IsNull() )
                continue;

            ++cnt;
            Paragraphs text;
            {ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
                text.LineWidth= LineWidth;
                text.Buffer._(NEW_LINE);
                text.AddMarked( comment );
            }
            section.Comments.Allocate(iniFile->Allocator, text.Buffer);
        }
    return cnt;
}

bool   IniFileFeeder::SetWriteBackFlag(const String& path)
{
    auto entry= SearchEntry( path );
    ALIB_ASSERT_WARNING( entry.second , "CONFIG", "Variable {!Q} to mark as 'writeback' not found.",
                                        path )
    if(  entry.second && entry.second->RawValue.IsEmpty() )
    {
        entry.second->WriteBack= true;
        return true;
    }
    return false;
}

bool   IniFileFeeder::SetWriteBackFlag( const Variable& var )
{
    ALIB_ASSERT_ERROR( var.IsDeclared(), "CONFIG", "Given Variable not declared." )
    ALIB_ASSERT_ERROR( &var.GetConfiguration() == &configuration, "CONFIG",
                        "Variable belongs to different configuration: ", String256(var))
    return SetWriteBackFlag( String256(var) );
}


} // namespace [alib::config]

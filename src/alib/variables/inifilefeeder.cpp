//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/variables/variables.prepro.hpp"
#include <fstream>
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Variables.IniFile;
    import   ALib.Strings.StdIOStream;
#  if ALIB_EXCEPTIONS
    import   ALib.Exceptions;
#  endif
    import   ALib.System;
    import   ALib.Variables;
#  if ALIB_FORMAT
    import   ALib.Format;
    import   ALib.Format.Paragraphs;
#  endif
#else
#   include "ALib.Strings.StdIOStream.H"
#   include "ALib.Exceptions.H"
#   include "ALib.Containers.StringTree.H"
#   include "ALib.Containers.StringTreeIterator.H"
#   include "ALib.System.H"
#   include "ALib.Variables.H"
#   include "ALib.Format.H"
#   include "ALib.Format.Paragraphs.H"
#   include "ALib.Variables.IniFile.H"
#endif
//========================================== Implementation ========================================

namespace alib::variables {

//##################################################################################################
// helpers
//##################################################################################################
std::pair<IniFile::Section*, IniFile::Entry*>   IniFileFeeder::SearchEntry ( const String& path ) {
    if(iniFile == nullptr) {
        ALIB_ERROR( "VARIABLES", "No INI-file loaded when trying to search data." )
        return std::pair<IniFile::Section*, IniFile::Entry*>(nullptr, nullptr);
    }

    // separate section/entry name
    auto sectionSeparator= path.LastIndexOf( configuration.Separator() );
    String sectionName= (sectionSeparator != -1) ? path.Substring<NC>(0, sectionSeparator ) : EMPTY_STRING;
    String entryName  = (sectionSeparator != -1) ? path.Substring<NC>(sectionSeparator + 1, path.Length() - sectionSeparator - 1) : path;

    // search for existing entry
    return iniFile->SearchEntry(sectionName, entryName);
}

std::pair<IniFile::Section*, IniFile::Entry*>   IniFileFeeder::SearchEntry ( const Variable& var ) {
    ALIB_ASSERT_ERROR( var.IsDeclared(), "VARIABLES", "Given Variable not declared." )
    ALIB_ASSERT_ERROR( &var.GetConfiguration() == &configuration, "VARIABLES",
                        "Variable belongs to different configuration: ", var )
    return SearchEntry( String256( var ) );
}

//##################################################################################################
// Import interface
//##################################################################################################
int     IniFileFeeder::ImportSection(  const String& sectionName, const String& typeName ) {
    if(iniFile == nullptr) {
        ALIB_ERROR( "VARIABLES", "No INI-file loaded when trying to import data." )
        return 0;
    }

    auto* section= iniFile->SearchSection( sectionName );
    if(section == nullptr) {
        ALIB_WARNING( "VARIABLES", "Section named \"{}\" not found in INI-file.", sectionName )
        return 0;
    }

    int cnt= 0;
    String256 varName;

    varName.Reset(sectionName);
    if( varName.IsNotEmpty())
        varName << configuration.Separator();

    // loop over all entries
    for ( auto& entry : section->Entries ) {
        StringLengthResetter sectionNameResetter(varName);
        varName << entry.Name;
        Variable var(configuration, varName, typeName);
        if( var.Define( priority) ) {
            var.Import(entry.Value, priority, &configuration.Escaper );
            ++cnt;
    }   }

    return cnt;
}

int     IniFileFeeder::importSection( IniFile::Section& section ) {
    int cnt= 0;
    String256 varName;

    varName.Reset(section.Name);
    if( varName.IsNotEmpty())
        varName << configuration.Separator();

    // loop over all entries
    for ( auto& entry : section.Entries ) {
        // Try if variable is declared and has lower or equal priority than us.
        StringLengthResetter sectionNameResetter(varName);
        varName << entry.Name;
        Variable var(configuration);
        Substring value= entry.Value;
        if( var.Try(varName) ) {
            var.Import(value, priority, &configuration.Escaper );
            ++cnt;
            continue;
        }

        // Variable not declared. Copy value to undeclared input variables
        configuration.PresetImportString(varName, value, &configuration.Escaper, priority);
    }

    return cnt;
}

int     IniFileFeeder::ImportSection(  const String& sectionName ) {
    if(iniFile == nullptr) {
        ALIB_ERROR( "VARIABLES", "No INI-file loaded when trying to import data." )
        return 0;
    }

    auto* section= iniFile->SearchSection( sectionName );
    if(section == nullptr) {
        ALIB_WARNING( "VARIABLES", "Section name \"{}\" not found in INI-file.", sectionName )
        return 0;
    }

    return importSection(*section);
}

int     IniFileFeeder::ImportAll() {
    if(iniFile == nullptr) {
        ALIB_ERROR( "VARIABLES", "No INI-file loaded when trying to import data." )
        return 0;
    }

    int cnt= 0;
    String256 varName;

    // loop over all sections
    for ( IniFile::Section& section : iniFile->Sections )
        cnt+= importSection( section );

    return cnt;
}

//##################################################################################################
// Export interface
//##################################################################################################
bool         IniFileFeeder::Export( const Variable& var) {
    if(iniFile == nullptr) {
        ALIB_ERROR( "VARIABLES", "No INI-file loaded when trying to export data." )
        return false;
    }
    ALIB_ASSERT_ERROR(var.IsDeclared(), "VARIABLES", "Variable to export not declared: ", var)

    String256 name(var);

    // separate section/entry name
    auto sectionSeparator= name.LastIndexOf( var.GetConfiguration().Separator() );
    String sectionName= (sectionSeparator != -1) ? name.Substring<NC>(0, sectionSeparator ) : EMPTY_STRING;
    String entryName  = (sectionSeparator != -1) ? name.Substring<NC>(sectionSeparator + 1, name.Length() - sectionSeparator - 1) : name;

    // search for existing entry
    auto pair= iniFile->SearchEntry(sectionName, entryName);
    auto* entry= pair.second;
    if( entry ) {
        // exists and no write back?
        if( !entry->WriteBack && !pair.first->WriteBack )
            return false;
    } else {
        // create entry
        auto sectionIt= iniFile->SearchOrCreateSection( sectionName );
        entry  = iniFile->CreateEntry( sectionIt.first, entryName );
    }
    
    {String4K buf;
        buf.DbgDisableBufferReplacementWarning();
        var.Export( buf, &configuration.Escaper );
        entry->NewValue.Allocate(iniFile->Allocator, buf );
    }

    // add comments
    if(entry->Comments.IsNull()) {
        auto* decl= var.GetDeclaration();
        if( decl && decl->Comments().IsNotEmpty() )
            iniFile->AddComments( entry->Comments, decl->Comments(), DefaultCommentPrefix );
    }

    return true;
}

int  IniFileFeeder::ExportSubTree( Configuration::Cursor cursor, bool directChildrenOnly ) {
    if(iniFile == nullptr) {
        ALIB_ERROR( "VARIABLES", "No INI-file loaded when trying to export data." )
        return 0;
    }
    int cnt= 0;
    // check cursor itself first
    if( !cursor.IsRoot() ) {
        Variable var(cursor);
        if( var.IsDeclared() )
            if( Export( var ) )
                cnt++;
    }

    StringTreeIterator<Configuration> stit;
    stit.SetPathGeneration( lang::Switch::On );
    stit.SetMaxDepth( directChildrenOnly ? 0 : (std::numeric_limits<unsigned>::max)() );
    stit.Initialize( cursor, lang::Inclusion::Exclude );
    while ( stit.IsValid() ) {
        if( stit.Node().Name().Equals(A_CHAR("$PRESETS")) ) {
            stit.NextSibling();
            continue;
        }

        Variable var(stit.Node());
        if( var.IsDeclared() && var.IsDefined() ) {
            if( Export( var ) )
                cnt++;
        }
        stit.Next();
    }
    return cnt;
}

#if ALIB_RESOURCES
int  IniFileFeeder::AddResourcedSectionComments( ResourcePool&    resourcePool,
                                                 const NString&   resourceCategory,
                                                 const NString&   resourceNamePrefix    ) {
    if(iniFile == nullptr) {
        ALIB_ERROR( "VARIABLES", "No INI-file loaded when trying to import data." )
        return 0;
    }

    // add section comments from resources to INI-file
    int cnt= 0;
    for( auto& section : iniFile->Sections )
        if( section.Comments.IsNull() ) {
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
#endif

bool   IniFileFeeder::SetWriteBackFlag(const String& path) {
    auto entry= SearchEntry( path );
    ALIB_ASSERT_WARNING( entry.second , "VARIABLES",
        "Variable \"{}\" to be marked as 'writeback' not found.",  path )

    if(  entry.second && entry.second->RawValue.IsEmpty() ) {
        entry.second->WriteBack= true;
        return true;
    }
    return false;
}

bool   IniFileFeeder::SetWriteBackFlag( const Variable& var ) {
    ALIB_ASSERT_ERROR( var.IsDeclared(), "VARIABLES", "Given Variable not declared." )
    ALIB_ASSERT_ERROR( &var.GetConfiguration() == &configuration, "VARIABLES",
                        "Variable belongs to different configuration: ", var)
    return SetWriteBackFlag( String256(var) );
}


} // namespace [alib::variables]

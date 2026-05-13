
namespace alib::variables {

//##################################################################################################
// helpers
//##################################################################################################
IniFile::Handle   IniFileFeeder::SearchEntry ( const String& path ) {
    if(iniFile == nullptr) {
        ALIB_ERROR( "VARIABLES", "No INI-file loaded when trying to search data." )
        return IniFile::Handle{nullptr, nullptr};
    }

    // separate section/entry name
    auto sectionSeparator= path.LastIndexOf( configuration.Separator() );
    String sectionName= (sectionSeparator != -1) ? path.Substring<NC>(0, sectionSeparator ) : EMPTY_STRING;
    String entryName  = (sectionSeparator != -1) ? path.Substring<NC>(sectionSeparator + 1, path.Length() - sectionSeparator - 1) : path;

    // search for existing entry
    return iniFile->SearchEntry(sectionName, entryName);
}

IniFile::Handle   IniFileFeeder::SearchEntry ( const Variable& var ) {
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
    auto handle= iniFile->SearchEntry(sectionName, entryName);
    auto* entry= handle.EntryPointer;
    if( entry ) {
        // exists and no write back?
        if(    !entry                ->WriteBack
            && !handle.SectionPointer->WriteBack )
            return false;
    } else {
        // create entry
        auto sectionIt= iniFile->SearchOrCreateSection( sectionName );
        entry  = iniFile->CreateEntry( sectionIt.first, entryName );
        if ( var.IsWriteBack() )
            entry->WriteBack= true;
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
    StringTreeIterator<Configuration> stit;
    stit.SetPathGeneration( lang::Switch::On );
    stit.SetMaxDepth( directChildrenOnly ? 1 : (std::numeric_limits<unsigned>::max)() );
    stit.Initialize( cursor, lang::Inclusion::Include );
    while ( stit.IsValid() ) {
        if ( stit.Node().IsRoot()) {
            stit.Next();
            continue;
        }
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
            {ALIB_LOCK_RECURSIVE_WITH(Formatter::DEFAULT_LOCK)
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
    auto handle= SearchEntry( path );
    ALIB_ASSERT_WARNING( handle.EntryPointer , "VARIABLES",
        "Variable \"{}\" to be marked as 'writeback' not found.",  path )

    if(  handle.EntryPointer && handle.EntryPointer->RawValue.IsEmpty() ) {
        handle.EntryPointer->WriteBack= true;
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

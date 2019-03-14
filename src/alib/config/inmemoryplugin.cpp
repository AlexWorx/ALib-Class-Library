// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_CONFIG_INMEMORY_PLUGIN)
    #include "alib/config/inmemoryplugin.hpp"
#endif


namespace aworx { namespace lib { namespace config {

// #################################################################################################
// interface
// #################################################################################################

InMemoryPlugin::~InMemoryPlugin()
{
    for ( Section* section : Sections )
        delete section;
}

void  InMemoryPlugin::Reset()
{
    for ( Section* section : Sections )
        delete section;
    Sections.clear();
    Sections.insert( Sections.end(), createSection( nullptr ) );
}

InMemoryPlugin::Section* InMemoryPlugin::SearchSection( const String& sectionName ) const
{
    if ( sectionName.IsEmpty() )
        return Sections[0];

    auto sIt= Sections.begin();
    while( sIt != Sections.end() )
    {
        if( (*sIt)->Name.Equals<Case::Ignore>( sectionName ) )
            return *sIt;
        sIt++;
    }
    return nullptr;
}

InMemoryPlugin::Section* InMemoryPlugin::SearchOrCreateSection( const String& sectionName, const String& comments )
{
    Section* s= SearchSection( sectionName );
    if ( s == nullptr )
        Sections.insert( Sections.end(), s= createSection( sectionName ) );

    if ( s->Comments.IsEmpty() )
        s->Comments._( comments );

    return s;
}


// #################################################################################################
// Configuration plug-in interface implementation
// #################################################################################################

bool  InMemoryPlugin::Load( Variable& variable, bool searchOnly ) const
{
    ALIB_ASSERT_WARNING( variable.Name.IsNotEmpty(), "Empty name given" )

    Section*    section;
    Entry*      entry=  nullptr;
    if (    (section= SearchSection     ( variable.Category     ) ) != nullptr
         && (entry=   section->GetEntry ( variable.Name, false  ) ) != nullptr
         && !searchOnly                                                            )
    {
        entry->ToVariable( *this, variable );
    }

    return entry != nullptr;
}

bool  InMemoryPlugin::Store( Variable& variable  )
{
    Section*    section=    SearchOrCreateSection( variable.Category, nullptr );
    if ( variable.Size() > 0 )
    {
        Entry* entry= section->GetEntry( variable.Name, true );
        entry->FromVariable( *this, variable );
        return true;
    }

    return  section->DeleteEntry( variable.Name );
}


// #################################################################################################
// class InMemoryPlugin::Section
// #################################################################################################
bool InMemoryPlugin::Section::DeleteEntry ( const String& entryName )
{
    for ( size_t idx= 0; idx < Entries.size(); ++idx )
    {
        Entry* entry= Entries[idx];
        if ( entry->Name.Equals<Case::Ignore>( entryName ) )
        {
            delete Entries[idx];
            Entries.erase( Entries.begin() + static_cast<std::vector<Entry*>::difference_type>(idx) );
            return true;
        }
    }
    return false;
}

InMemoryPlugin::Entry* InMemoryPlugin::Section::GetEntry( const String& entryName, bool create )
{
    if ( entryName.IsEmpty() )
    {
        ALIB_WARNING( "Empty variable name given" )
        return nullptr;
    }

    auto entryIt= Entries.begin();
    while( entryIt != Entries.end() )
    {
        if( (*entryIt)->Name.Equals<Case::Ignore>( entryName ) )
            return *entryIt;
        entryIt++;
    }

    if ( !create )
        return nullptr;

    // create entry
    return *Entries.insert( Entries.end(), createEntry( entryName ) );
}

// #################################################################################################
// class InMemoryPlugin::Entry
// #################################################################################################
void InMemoryPlugin::Entry::ToVariable( const InMemoryPlugin& , Variable& variable )
{
    ALIB_ASSERT( Delim != '\0' || Values.size() <= 1);
    if ( Delim != '\0' )                        variable.Delim=               Delim;
    if ( FmtHints != FormatHints::None )        variable.FmtHints=            FmtHints;
    if ( FormatAttrAlignment.IsNotEmpty() )     variable.FormatAttrAlignment= FormatAttrAlignment;

    variable.Comments.Reset( Comments );
    for( AString& val : Values )
        variable.Add( val );
}


void InMemoryPlugin::Entry::FromVariable( const InMemoryPlugin& , Variable& variable )
{
    // copy attributes
    Delim=               variable.Delim;
    FmtHints=            variable.FmtHints;
    FormatAttrAlignment= variable.FormatAttrAlignment;

    if ( Comments.IsEmpty() )    // do not overwrite comments
        Comments._( variable.Comments );

    // adjust size of value array
    int varSize=                   variable.Size();
    int valSize= static_cast<int>( Values.size()   );
    while (valSize < varSize )
    {
        Values.emplace_back( nullptr );
        valSize++;
    }
    if (valSize > varSize )
        Values.erase( Values.begin() + varSize, Values.end() );

    // copy
    for ( int i= 0; i< varSize; i++ )
        Values[static_cast<size_t>(i)].Reset( variable.GetString( i ) );
}


// #################################################################################################
// CLIArgs Iterator
// #################################################################################################
//! @cond NO_DOX
class InMemoryPlugin::InMemoryPluginIteratorImpl : public ConfigurationPlugin::Iterator
{
    InMemoryPlugin& parent;
    InMemoryPlugin::Section*                      section;
    std::vector<InMemoryPlugin::Entry*>::iterator actualEntry;


    public:

    InMemoryPluginIteratorImpl( InMemoryPlugin& plugin, const String& sectionName )
    : parent(plugin)
    {
        if( (section= parent.SearchSection( sectionName )) !=nullptr )

            actualEntry= section->Entries.begin();
    }

    virtual ~InMemoryPluginIteratorImpl() override
    {}

    virtual bool        Next( Variable& variable )                                          override
    {
        // clear variable name at least. Values remain, until something was found. The caller has
        // to check the result anyhow!
        variable.Name.Reset();

        if( section == nullptr || actualEntry == section->Entries.end() )
            return false;

        variable.Declare( section->Name,  (*actualEntry)->Name );
        (*actualEntry)->ToVariable( parent, variable );
        actualEntry++;
        return true;
    }
};


ConfigurationPlugin::Iterator*   InMemoryPlugin::GetIterator( const String& sectionName )
{
    return new InMemoryPluginIteratorImpl( *this, sectionName );
}
//! @endcond



}}}// namespace [aworx::lib::config]


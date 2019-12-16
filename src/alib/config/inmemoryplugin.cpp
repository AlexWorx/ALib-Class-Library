// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_CONFIG_INMEMORY_PLUGIN)
    #include "alib/config/inmemoryplugin.hpp"
#endif
#endif // !defined(ALIB_DOX)



namespace aworx { namespace lib { namespace config {

// #################################################################################################
// interface
// #################################################################################################

InMemoryPlugin::InMemoryPlugin( String pName, MonoAllocator* externalMA )
: ConfigurationPlugin()
, allocator ( externalMA ? externalMA : MonoAllocator::Create(8 * 1024) )
, plugInName( allocator->EmplaceString(pName) )
, entryTable( allocator )
, sections  ( allocator )
, isMAOwner ( externalMA == nullptr )
{
    #if ALIB_DEBUG_MONOMEM
        if( externalMA == nullptr )
            allocator->LogDomain= A_CHAR("MA/CFG/IMPLGN");
    #endif
}

InMemoryPlugin::~InMemoryPlugin()
{
    sections.Reset();
    entryTable.Reset();
    if( isMAOwner )
        monomem::Destruct( allocator );
}

void  InMemoryPlugin::Clear()
{
    entryTable.Clear();
    sections  .Clear();
}

const InMemoryPlugin::Section* InMemoryPlugin::SearchSection( const String& sectionName )
{
    if( sections.IsEmpty() )
        createSection( EmptyString()  );

    auto sIt= sections.begin();
    while( sIt != sections.end() )
    {
        if( sIt->Name().Equals<Case::Ignore>( sectionName ) )
            return &*sIt;
        ++sIt;
    }
    return nullptr;
}

std::pair<InMemoryPlugin::Section*, bool>
    InMemoryPlugin::SearchOrCreateSection( const String& sectionName )
{
    Section* s= const_cast<Section*>( SearchSection( sectionName ) );
    if ( s != nullptr )
        return std::make_pair( s, false );

    return std::make_pair( createSection( sectionName ), true );
}

// #################################################################################################
// Configuration plug-in interface implementation
// #################################################################################################

bool  InMemoryPlugin::Load( Variable& variable, bool searchOnly )
{
    ALIB_ASSERT_WARNING( variable.Name().IsNotEmpty(), "Empty name given" )
    auto* entry=  searchEntry( variable.Category(), variable.Name() );
    if( entry != nullptr && !searchOnly )
        ToVariable( *entry, variable );

    return entry != nullptr;
}

bool  InMemoryPlugin::Store( Variable& variable  )
{
    auto it=  entryTable.Find( EntryKey(variable.Category(), variable.Name()) );

    // exists
    if( it != entryTable.end() )
    {
        // delete if given variable is nulled
        if ( variable.Size() == 0 )
        {
            auto handle= entryTable.Extract( it );   ALIB_ASSERT( !handle.IsEmpty() )
            handle.Mapped().first->entries.Erase( handle.Mapped().second );
        }
        else
            FromVariable( *it.Mapped().second, variable );

        return true; // written (or deleted)
    }

    // does not exist, yet
    if ( variable.Size() == 0 )
        return false; // not written

    // search section and create there
    Section*    section=    SearchOrCreateSection( variable.Category() ).first;
    auto* entry= createEntry( section, variable.Name() );
    FromVariable( *entry, variable );
    return true;
}


void InMemoryPlugin::ToVariable( Entry& entry, Variable& variable )                            const
{
    ALIB_ASSERT( entry.Delim != '\0' || entry.AdditionalValues.Size() == 0)
    if ( entry.Delim              != '\0'              )  variable.SetDelim                  ( entry.Delim               );
    if ( entry.FmtHints           != FormatHints::None )  variable.SetFmtHints               ( entry.FmtHints            );
    if ( entry.FormatAttrAlignment.IsNotEmpty()        )  variable.ReplaceFormatAttrAlignment( entry.FormatAttrAlignment );
    if ( entry.Comments           .IsNotEmpty()        )  variable.ReplaceComments           ( entry.Comments            );

    if( entry.Value.IsNotNull() )
    {
        variable.Add( entry.Value );
        for( const AString& val : entry.AdditionalValues )
            variable.Add( val );
    }
}

void InMemoryPlugin::FromVariable( Entry& entry, Variable& variable )                          const
{
    // copy attributes
    entry.Delim=               variable.Delim();
    entry.FmtHints=            variable.FmtHints();
    entry.FormatAttrAlignment= variable.FormatAttrAlignment();

    if ( entry.Comments.IsEmpty() )    // do not overwrite comments
        entry.Comments._( variable.Comments() );

    // adjust size of value array and copy values
    integer varSize=  variable.Size();
    auto valueIt= entry.SetValueCount( varSize );

    entry.Value.Reset( variable.GetString( 0 ) );
    int idx= 1;
    while( valueIt != entry.AdditionalValues.end() )
    {
        valueIt->Reset( variable.GetString( idx++ ) );
        ++valueIt;
    }
}


// #################################################################################################
// CLIArgs Iterator
// #################################################################################################
//! @cond NO_DOX
class InMemoryPluginIteratorImpl : public ConfigurationPlugin::Iterator
{
    InMemoryPlugin&                        parent;
    InMemoryPlugin::Section*               section;
    InMemoryPlugin::TEntryList::Iterator   actualEntry;


    public:

    InMemoryPluginIteratorImpl( InMemoryPlugin& plugin, const String& sectionName )
    : parent(plugin)
    {
        if( (section= const_cast<InMemoryPlugin::Section*>( parent.SearchSection( sectionName ))) !=nullptr )
            actualEntry= section->entries.begin();
    }

    virtual ~InMemoryPluginIteratorImpl() override
    {}

    virtual bool        Next( Variable& variable )                                          override
    {
        if( section == nullptr || actualEntry == section->entries.end() )
            return false;

        variable.Declare( section->Name(),  actualEntry->Name() );
        parent.ToVariable( *actualEntry, variable );
        ++actualEntry;
        return true;
    }
};

ConfigurationPlugin::Iterator*   InMemoryPlugin::GetIterator( const String& sectionName )
{
    return new InMemoryPluginIteratorImpl( *this, sectionName );
}
//! @endcond



}}}// namespace [aworx::lib::config]


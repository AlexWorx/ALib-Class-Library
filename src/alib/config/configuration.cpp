// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/config/configuration.hpp"
#   include "alib/config/plugins.hpp"
#   include "alib/config/configcamp.hpp"
#   include "alib/lang/message/report.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"
#endif // !DOXYGEN

namespace alib {

//==================================================================================================
/// This is the namespace of\alibmod, which implements access, evaluation and writing of
/// configuration variables. Variable data can be load from different sources, for example
/// configuration files, command line parameters, environment variables, or any custom storage type.
///
/// Please consult \ref alib_mod_config "ALib Module Config - Programmer's Manual"
/// for further information.
//==================================================================================================
namespace config {


// =================================================================================================
//  Implementation of class detail::NodeMaintainer
// =================================================================================================
namespace detail
{
void ConfigNodeHandler::FreeNode( TTree& tree, typename TTree::Node& node )
{
    // delete node name
    auto& cfg= static_cast<Configuration&>(tree);
    cfg.Pool.free( const_cast<TTree::CharacterType*>(node.name.storage.Buffer()),
                                                       size_t(node.name.storage.Length()) * sizeof(TTree::CharacterType) );

    // delete vdata
    Entry& entry= node.data;
    if( entry.data  )
    {
        entry.meta->destruct(entry.data, cfg.Pool);
        cfg.Pool().Free(entry.data, entry.meta->size() );
    }
}
} // namespace detail

// =================================================================================================
//  Implementation of class Configuration
// =================================================================================================
Configuration::Configuration( MonoAllocator& allocator, lang::CreateDefaults createDefaults )
: StringTree             ( allocator, '/')
, Pool                   ( allocator )
, types                  ( allocator )
, replacementDeclarations( allocator )
, listeners              ( allocator )
, BooleanTokens          ( allocator )
{
    DbgSetDCSName("Configuration");

    // Register built-in types
    registerType<alib::config::detail::VMeta_Bool                 >();
    registerType<alib::config::detail::VMeta_integer              >();
    registerType<alib::config::detail::VMeta_float               >();
    registerType<alib::config::detail::VMeta_Box                  >();
    registerType<alib::config::detail::VMeta_String               >();
    registerType<alib::config::detail::VMeta_StringVectorComma    >();
    registerType<alib::config::detail::VMeta_StringVectorSemicolon>();

    // read boolean false/true values from resources
    if( createDefaults == lang::CreateDefaults::Yes)
    {
        Token tokenBuf[10];
        Token::LoadResourcedTokens( CONFIG, "BTF", tokenBuf ALIB_DBG(, 10) );
        for (int i = 0; i < 10; i+= 2 )
            BooleanTokens.EmplaceBack(tokenBuf[i], tokenBuf[i+1]);

        // create default plugins
        auto& ma= GetAllocator();
        InsertPlugin( environmentPlugin= ma().New<EnvironmentVariablesPlugin>(ma) );
        InsertPlugin(         cliPlugin= ma().New<        CLIVariablesPlugin>(ma) );
    }
}

Configuration::~Configuration()
{
    // we have to delete all nodes before the invocation of the base destructor, because
    // this would use our pool allocator on existing nodes (which is then destructed already).
    base::Clear();

    ALIB_ASSERT_WARNING( listeners.IsEmpty(), "CONFIG",
         "Remaining registered listeners when destruction configuration.")
}

void Configuration::registerListener( ConfigurationListener*         listener,
                                          lang::ContainerOp              insertOrRemove,
                                          int                            event,
                                          const Variable*                variable,
                                          const StringTree::Cursor*      subTree,
                                          const String&                  variableName,
                                          const String&                  pathPrefixGiven,
                                          const String&                  pathSubstring  )
{
    // checks
    ALIB_ASSERT_ERROR( variable==nullptr ||  variable->IsDeclared()              , "CONFIG", "Given variable not declared.")
    ALIB_ASSERT_ERROR( variable==nullptr || &variable->AsCursor().Tree() == this , "CONFIG", "Given variable does not belong to this configuration.")
    ALIB_ASSERT_ERROR( subTree ==nullptr ||  subTree->IsValid()                  , "CONFIG", "Invalid cursor given."       )
    ALIB_ASSERT_ERROR( subTree ==nullptr || &subTree            ->Tree() == this , "CONFIG", "Given cursor does not belong to this configuration.")

    // remove / from path
    const String pathPrefix= pathPrefixGiven.CharAtStart() == StringTree::Separator()
                             ? pathPrefixGiven.Substring(1)
                             : pathPrefixGiven;

    // ---------------- registration ---------------------
    if( insertOrRemove == lang::ContainerOp::Insert)
    {
        listeners.EmplaceBack( ListenerRecord{ listener,
                                               event,
                                               variable ? variable->AsCursor().Export() : ConstCursorHandle(),
                                               subTree  ? subTree->            Export() : ConstCursorHandle(),
                                               AStringPA(Pool),
                                               AStringPA(Pool),
                                               AStringPA(Pool)   } );
        listeners.Back().variableName << variableName;
        listeners.Back().pathPrefix   << pathPrefix;
        listeners.Back().pathSubstring<< pathSubstring;

        return;
    }

    // ---------------- de-registration ---------------------
    for (auto it= listeners.begin() ; it != listeners.end() ; ++it )
        if(     it->listener == listener
            &&  it->event    == event
            &&  it->variable == ( variable ? variable->AsCursor().Export() : ConstCursorHandle() )
            &&  it->subTree  == ( subTree  ? subTree            ->Export() : ConstCursorHandle() )
            &&  it->variableName .Equals( variableName )
            &&  it->pathPrefix   .Equals( pathPrefix )
            &&  it->pathSubstring.Equals( pathSubstring )    )
        {
            (void) listeners.Erase( it );
            return;
        }

    ALIB_WARNING( "CONFIG", "Listener with matching set of parameters not found with deregistration." )

}  // Configuration::registerListener


int Configuration::MonitorStop( ConfigurationListener*  listener )
{
    // checks
    ALIB_ASSERT_ERROR( listener!=nullptr, "CONFIG", "Given listener is nullptr.")

    // ---------------- de-registration ---------------------
    int cnt= 0;
    for (auto it= listeners.begin() ; it != listeners.end() ; )
        if( it->listener == listener )
        {
            it= listeners.Erase( it );
            ++cnt;
        }
        else
             ++it;

    return cnt;
}  // Configuration::registerListener

void Configuration::notifyListeners(  int               event,
                                          const Variable&   variable,
                                          const String&     variablePathGiven,
                                          Priority          previousPriority  )
{
    String256       variablePathBuffer;
    const String*   variablePath= &variablePathGiven;
    for (auto it= listeners.begin() ; it != listeners.end() ; ++it )
        if( event == it->event )
        {
            // if needed generate variable path
            if(     variablePath->IsEmpty()
                &&  (   it->variableName .IsNotEmpty()
                     || it->pathPrefix   .IsNotEmpty()
                     || it->pathSubstring.IsNotEmpty() )     )
            {
                variablePathBuffer << variable;
                variablePath= &variablePathBuffer;
            }

            if(    ( it->variable     .IsValid()    && ( it->variable ==  variable.AsCursor().Export() ) )
                || ( it->subTree      .IsValid()    && ( variable.AsCursor().Distance( ImportCursor(it->subTree) ) >= 0 ) )
                || ( it->variableName .IsNotEmpty() && it->variableName.Equals(variable.AsCursor().Name()) )
                || ( it->pathPrefix   .IsNotEmpty() && variablePath->StartsWith(it->pathPrefix) )
                || ( it->pathSubstring.IsNotEmpty() && variablePath->IndexOf(it->pathSubstring) >= 0 )
               )
            {
                it->listener->Notify( variable,
                                      ConfigurationListener::Event(event),
                                      previousPriority                        );
            }
     }
} // Configuration::notifyListeners


void Configuration::presetImportString(const String& name, const String& value,
                                                   const StringEscaper* escaper,   Priority priority)
{
    auto cursor= Root();
    cursor.GoToCreateChildIfNotExistent(A_CHAR("$PRESETS"));

    // nullptr given? Delete a preset.
    if( value.IsNull() )
    {
        if( cursor.GoTo(name).IsNotEmpty() )
            return; // nothing was previously set

        auto& entry= *cursor;
        if( entry.priority > priority )
            return; // do not delete if lower priority!

        if( entry.data )
        {
            entry.meta->destruct( entry.data, Pool );
            Pool().Free( entry.data, entry.meta->size() );
            entry.meta= nullptr;
            entry.data= nullptr;
            entry.priority= Priority::NONE;
        }

        // delete this node, but only if it is a leaf.
        if( !cursor.HasChildren() )
            cursor.Delete();

        return;
    }


    // set-mode
    cursor.GoToCreatedPathIfNotExistent(name);

    // create a fake variable. We do not do this using the variable class, because this would
    // raise assertions.
    auto& entry= *cursor;
    if( entry.data == nullptr )
    {
        auto it= types.Find(A_CHAR("S"));
        ALIB_ASSERT_ERROR( it != types.end(), "CONFIG",
                           "Variable type 'S' not registered. This usually cannot happen." )
        auto*  meta= entry.meta= *it;

        // initialize fields
        entry.data   =  reinterpret_cast<detail::VDATA*>(Pool().Alloc( meta->size(), alignof(detail::VDATA)));
        meta->construct( cursor->data, Pool );
        entry.priority= priority;
    }

    if( entry.priority <= priority )
    {
        entry.priority   = priority;
        entry.declaration= reinterpret_cast<const Declaration*>( escaper );
        (Variable(cursor))= value;
    }
}

// #############################################################################################
// Declaration replacement allocation
// #############################################################################################
const Declaration* Configuration::StoreDeclaration( const Declaration* orig, const Box& replacements )
{
    //------------- prepare replacement -------------
    String128 replace;

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    const Box*  replacementPtr;
    integer     qtyReplacements;
         if ( replacements.IsArrayOf<Box>() )
    {
        replacementPtr = replacements.UnboxArray<Box>();
        qtyReplacements= replacements.UnboxLength();
    }
    else if ( replacements.IsType<BoxesHA*>() )
    {
        const auto* boxes= replacements.Unbox<BoxesHA*>();
        replacementPtr = boxes->data();
        qtyReplacements= boxes->Size();
    }
    else if ( replacements.IsType<BoxesMA*>() )
    {
        const auto* boxes= replacements.Unbox<BoxesMA*>();
        replacementPtr = boxes->data();
        qtyReplacements= boxes->Size();
    }
    else if ( replacements.IsType<BoxesPA*>() )
    {
        const auto* boxes= replacements.Unbox<BoxesPA*>();
        replacementPtr = boxes->data();
        qtyReplacements= boxes->Size();
    }
    else
    {
        replacementPtr = &replacements;
        qtyReplacements= 1;
    }

    //------------- replace name -------------
    String256 bufName;              ALIB_DBG( bufName    .DbgDisableBufferReplacementWarning() );
    bufName         << orig->EnumElementName;

    for ( integer  replCnt= 0; replCnt< qtyReplacements ; ++replCnt )
        if ( !replacementPtr->IsType<void>() )
        {
            String64  search("%"); search._( replCnt + 1 );
            replace.Reset( *( replacementPtr + replCnt) );
            bufName    .SearchAndReplace( search, replace );
        }

    // search in hashtable
    {
        auto it= replacementDeclarations.Find( bufName );
        if( it != replacementDeclarations.end() )
            return *it;
    }

    // not found, we have to create a new one
    String1K  bufComments;       ALIB_DBG( bufComments    .DbgDisableBufferReplacementWarning() );
    String128 bufDefaultValue;   ALIB_DBG( bufDefaultValue.DbgDisableBufferReplacementWarning() );
    bufComments         << orig->comments;
    if ( orig->defaultValue.IsNotNull() )
        bufDefaultValue << orig->defaultValue;

    for ( integer  replCnt= 0; replCnt< qtyReplacements ; ++replCnt )
        if ( !replacementPtr->IsType<void>() )
        {
            String64  search("%"); search._( replCnt + 1 );
            replace.Reset( *( replacementPtr + replCnt) );
            bufComments    .SearchAndReplace( search, replace );
            bufDefaultValue.SearchAndReplace( search, replace );
        }

    ALIB_WARNINGS_RESTORE

    // --------------------- create copy ---------------------
    Declaration* result    = GetAllocator()().New<Declaration>();
    result->EnumElementName.Allocate(GetAllocator(), bufName);
    result->typeName       = orig->typeName;
    result->defaultValue   .Allocate(GetAllocator(), bufDefaultValue);
    result->comments       .Allocate(GetAllocator(), bufComments);
    replacementDeclarations.EmplaceUnique(result);

    return result;
}

// #############################################################################################
// Other tools
// #############################################################################################
std::pair<bool,int8_t> Configuration::ParseBooleanToken( const String& pValue )
{
    int8_t index= 0;
    Substring value(pValue);
    if( value.Trim().IsEmpty() )
        return {false, int8_t(-1)};

    for ( auto& it : BooleanTokens )
    {
        if ( it.first .Match(value)  ) return {false, index};
        if ( it.second.Match(value)  ) return {true , index};
        ++index;
    }
    return {false, int8_t(-1)};
}

AString& Configuration::WriteBooleanToken( bool value, int8_t idxRequested, AString& dest )
{
    // find the right pair of tokens
    if(idxRequested < 0)
        idxRequested= 0;

    auto it= BooleanTokens.begin();
    for( int8_t index=0 ; index < idxRequested && it != BooleanTokens.end() ; ++index )
        ++it;

    if( it == BooleanTokens.end() )
        it= BooleanTokens.begin();

    // use first (false) or second (true) to write
    (!value ? it->first : it->second).GetExportName( dest );
    return dest;
}

bool Configuration::DeletePath( const String& path )
{
DOX_MARKER( [DOX_CONFIG_DELETE_SAMPLE] )
// get root node of the tree
Cursor cs= Root();

// try to walk the givedn path. If a remainder of the string exits, exit.
if( cs.GoTo(path).IsNotEmpty() )
    return false;

// now delete the subtree, including the node the cursor represents
cs.Delete();

// that's it
return true;
DOX_MARKER( [DOX_CONFIG_DELETE_SAMPLE] )
}

}} // namespace [alib::config]


// #################################################################################################
// struct T_Append<Variable>
// #################################################################################################
#if !DOXYGEN

namespace alib::strings {

void T_Append<config::Variable,nchar, lang::HeapAllocator>::operator()( TAString<nchar, lang::HeapAllocator>& target, const config::Variable& variable )
{
#if ALIB_CHARACTERS_WIDE
    String256 name;
    variable.Name(name);
    target << name;
#else
    variable.Name(target);
#endif

}

void T_Append<config::Variable,wchar, lang::HeapAllocator>::operator()( TAString<wchar, lang::HeapAllocator>& target, const config::Variable& variable )
{
#if ALIB_CHARACTERS_WIDE
    variable.Name(target);
#else
    String256 name;
    variable.Name(name);
    target << name;
#endif
}


} // namespace [alib::strings]
#endif // ALIB_DOX



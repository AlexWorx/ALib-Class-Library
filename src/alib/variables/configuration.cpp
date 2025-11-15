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
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Variables;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Boxing;
#  if ALIB_RESOURCES
    import   ALib.Resources;
#  endif
    import   ALib.Variables.Plugins;
#  if ALIB_CAMP
    import   ALib.Camp;
#  endif
    import   ALib.Camp.Base;
#else
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Boxing.H"
#   include "ALib.Resources.H"
#   include "ALib.Variables.Plugins.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.Variables.H"
#endif
//========================================== Implementation ========================================

ALIB_BOXING_VTABLE_DEFINE( alib::variables::Priority                  , vt_config_priorities)
ALIB_BOXING_VTABLE_DEFINE( alib::variables::Variable                  , vt_config_variable    )
ALIB_BOXING_VTABLE_DEFINE( alib::variables::Configuration::ConstCursor, vt_config_constcursor )


namespace alib {

//==================================================================================================
/// This is the namespace of\alibmod, which implements access, evaluation and writing of
/// configuration variables. Variable data can be load from different sources, for example
/// configuration files, command line parameters, environment variables, or any custom storage type.
///
/// Please consult \ref alib_mod_variables "ALib Module Config - Programmer's Manual"
/// for further information.
//==================================================================================================
namespace variables {


//==================================================================================================
//  Implementation of class detail::NodeMaintainer
//==================================================================================================
namespace detail
{
void ConfigNodeHandler::FreeNode( typename TTree::Node& node, TTree& tree ) {
    // delete node name
    auto& cfg= static_cast<Configuration&>(tree);
    cfg.Pool.free( const_cast<TTree::CharacterType*>(node.name.storage.Buffer()),
                                                       size_t(node.name.storage.Length()) * sizeof(TTree::CharacterType) );

    // delete vdata
    Entry& entry= node.data;
    if( entry.data  ) {
        entry.meta->destruct(entry.data, cfg.Pool);
        cfg.Pool().Free(entry.data, entry.meta->size() );
}   }
} // namespace detail

//==================================================================================================
//  Implementation of class Configuration
//==================================================================================================
Configuration::Configuration( MonoAllocator& allocator, lang::CreateDefaults createDefaults )
: StringTree             ( allocator, '/')
, Pool                   ( allocator )
, types                  ( allocator )
, replacementDeclarations( allocator )
, listeners              ( allocator )
, BooleanTokens          ( allocator ) {
    DbgSetDCSName("Configuration");

    // Register built-in types
    registerType<alib::variables::detail::VMeta_Bool                 >();
    registerType<alib::variables::detail::VMeta_integer              >();
    registerType<alib::variables::detail::VMeta_float               >();
    registerType<alib::variables::detail::VMeta_Box                  >();
    registerType<alib::variables::detail::VMeta_String               >();
    registerType<alib::variables::detail::VMeta_StringVectorComma    >();
    registerType<alib::variables::detail::VMeta_StringVectorSemicolon>();

    // read boolean false/true values from resources
    if( createDefaults == lang::CreateDefaults::Yes) {
        std::array<Token, 10> tokenBuf;
        #if ALIB_CAMP
        strings::util::LoadResourcedTokens( BASECAMP, "CFGBTF", tokenBuf.data()  ALIB_DBG(, 10) );
        for (size_t i = 0; i < tokenBuf.size(); i+= 2 )
            BooleanTokens.emplace_back(tokenBuf[i], tokenBuf[i+1]);
        #else
            BooleanTokens.emplace_back(Token("False;I;1"), Token("True;I;1"));
            BooleanTokens.emplace_back(Token(    "0;I;1"), Token(   "1;I;1"));
            BooleanTokens.emplace_back(Token(   "No;I;1"), Token( "Yes;I;1"));
            BooleanTokens.emplace_back(Token(  "Off;I;3"), Token(  "On;I;2"));
            BooleanTokens.emplace_back(Token(  "---;I;1"), Token(  "OK;I;2" ));
        #endif

        // create default plugins
        auto& ma= GetAllocator();
        InsertPlugin( environmentPlugin= ma().New<EnvironmentVariablesPlugin>(ma) );
        InsertPlugin(         cliPlugin= ma().New<        CLIVariablesPlugin>(ma) );
}   }

Configuration::~Configuration() {
    // we have to delete all nodes before the invocation of the base destructor, because
    // this would use our pool allocator on existing nodes (which is then destructed already).
    base::Clear();

    ALIB_ASSERT_WARNING( listeners.empty(), "VARIABLES",
         "{} remaining registered listeners when destruction configuration.", listeners.size() )
}

void Configuration::registerListener( ConfigurationListener*         listener,
                                          lang::ContainerOp              insertOrRemove,
                                          int                            event,
                                          const Variable*                variable,
                                          const StringTree::Cursor*      subTree,
                                          const String&                  variableName,
                                          const String&                  pathPrefixGiven,
                                          const String&                  pathSubstring  ) {
    // checks
    ALIB_ASSERT_ERROR( variable==nullptr ||  variable->IsDeclared()              , "VARIABLES", "Given variable not declared.")
    ALIB_ASSERT_ERROR( variable==nullptr || &variable->AsCursor().Tree() == this , "VARIABLES", "Given variable does not belong to this configuration.")
    ALIB_ASSERT_ERROR( subTree ==nullptr ||  subTree->IsValid()                  , "VARIABLES", "Invalid cursor given."       )
    ALIB_ASSERT_ERROR( subTree ==nullptr || &subTree            ->Tree() == this , "VARIABLES", "Given cursor does not belong to this configuration.")

    // remove / from path
    const String pathPrefix= pathPrefixGiven.CharAtStart() == StringTree::Separator()
                             ? pathPrefixGiven.Substring(1)
                             : pathPrefixGiven;

  //------------------------------------------ registration ----------------------------------------
    if( insertOrRemove == lang::ContainerOp::Insert) {
        listeners.emplace_back( ListenerRecord{ listener,
                                               event,
                                               variable ? variable->AsCursor().Export() : ConstCursorHandle(),
                                               subTree  ? subTree->            Export() : ConstCursorHandle(),
                                               AStringPA(Pool),
                                               AStringPA(Pool),
                                               AStringPA(Pool)   } );
        listeners.back().variableName << variableName;
        listeners.back().pathPrefix   << pathPrefix;
        listeners.back().pathSubstring<< pathSubstring;

        return;
    }

  //----------------------------------------------- de ---------------------------------------------
    for (auto it= listeners.begin() ; it != listeners.end() ; ++it )
        if(     it->listener == listener
            &&  it->event    == event
            &&  it->variable == ( variable ? variable->AsCursor().Export() : ConstCursorHandle() )
            &&  it->subTree  == ( subTree  ? subTree            ->Export() : ConstCursorHandle() )
            &&  it->variableName .Equals( variableName )
            &&  it->pathPrefix   .Equals( pathPrefix )
            &&  it->pathSubstring.Equals( pathSubstring )    )
        {
            (void) listeners.erase( it );
            return;
        }

    ALIB_WARNING( "VARIABLES", "Listener with matching set of parameters not found with "
                               "deregistration." )

}  // Configuration::registerListener


int Configuration::MonitorStop( ConfigurationListener*  listener ) {
    // checks
    ALIB_ASSERT_ERROR( listener!=nullptr, "VARIABLES", "Given listener is nullptr.")

  //----------------------------------------------- de ---------------------------------------------
    int cnt= 0;
    for (auto it= listeners.begin() ; it != listeners.end() ; )
        if( it->listener == listener ) {
            it= listeners.erase( it );
            ++cnt;
        }
        else
             ++it;

    return cnt;
}  // Configuration::registerListener

void Configuration::notifyListeners(  int               event,
                                          const Variable&   variable,
                                          const String&     variablePathGiven,
                                          Priority          previousPriority  ) {
    String256       variablePathBuffer;
    const String*   variablePath= &variablePathGiven;
    for (auto it= listeners.begin() ; it != listeners.end() ; ++it )
        if( event == it->event ) {
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
    }       }
} // Configuration::notifyListeners


void Configuration::presetImportString( const String&        name,     const String& value,
                                        const StringEscaper* escaper,  Priority      priority ) {
    auto cursor= Root();
    cursor.GoToCreateChildIfNotExistent(A_CHAR("$PRESETS"));

    // nullptr given? Delete a preset.
    if( value.IsNull() ) {
        if( cursor.GoTo(name).IsNotEmpty() )
            return; // nothing was previously set

        auto& entry= *cursor;
        if( entry.priority > priority )
            return; // do not delete if lower priority!

        if( entry.data ) {
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
    if( entry.data == nullptr ) {
        auto it= types.Find(A_CHAR("S"));
        ALIB_ASSERT_ERROR( it != types.end(), "VARIABLES",
                           "Variable type 'S' not registered. This usually cannot happen." )
        auto*  meta= entry.meta= *it;

        // initialize fields
        entry.data   =  reinterpret_cast<detail::VDATA*>(Pool().Alloc( meta->size(), alignof(detail::VDATA)));
        meta->construct( cursor->data, Pool );
        entry.priority= priority;
    }

    if( entry.priority <= priority ) {
        entry.priority   = priority;
        entry.declaration= reinterpret_cast<const Declaration*>( escaper );
        (Variable(cursor))= value;
}   }

//##################################################################################################
// Declaration replacement allocation
//##################################################################################################
const Declaration* Configuration::StoreDeclaration( const Declaration* orig,
                                                    const Box&         replacements ) {
  //-------------------------------------- prepare replacement -------------------------------------
    String128 replace;

    const Box*  replacementPtr;
    integer     qtyReplacements;
         if ( replacements.IsArrayOf<Box>() ) {
        replacementPtr = replacements.UnboxArray<Box>();
        qtyReplacements= replacements.UnboxLength();
    }
    else if ( replacements.IsType<Boxes*>() ) {
        const auto* boxes= replacements.Unbox<Boxes*>();
        replacementPtr = boxes->data();
        qtyReplacements= boxes->Size();
    }
    else if ( replacements.IsType<BoxesMA*>() ) {
        const auto* boxes= replacements.Unbox<BoxesMA*>();
        replacementPtr = boxes->data();
        qtyReplacements= boxes->Size();
    }
    else if ( replacements.IsType<BoxesPA*>() ) {
        const auto* boxes= replacements.Unbox<BoxesPA*>();
        replacementPtr = boxes->data();
        qtyReplacements= boxes->Size();
    } else {
        replacementPtr = &replacements;
        qtyReplacements= 1;
    }

  //------------------------------------------ replace name ----------------------------------------
    String256 bufName;              ALIB_DBG( bufName    .DbgDisableBufferReplacementWarning() );
    bufName         << orig->EnumElementName;

    for ( integer  replCnt= 0; replCnt< qtyReplacements ; ++replCnt )
        if ( !replacementPtr->IsType<void>() ) {
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
        if ( !replacementPtr->IsType<void>() ) {
            String64  search("%"); search._( replCnt + 1 );
            replace.Reset( *( replacementPtr + replCnt) );
            bufComments    .SearchAndReplace( search, replace );
            bufDefaultValue.SearchAndReplace( search, replace );
        }

  //------------------------------------------ create copy -----------------------------------------
    Declaration* result    = GetAllocator()().New<Declaration>();
    result->EnumElementName.Allocate(GetAllocator(), bufName);
    result->typeName       = orig->typeName;
    result->defaultValue   .Allocate(GetAllocator(), bufDefaultValue);
    result->comments       .Allocate(GetAllocator(), bufComments);
    replacementDeclarations.EmplaceUnique(result);

    return result;
}

//##################################################################################################
// Other tools
//##################################################################################################
std::pair<bool,int8_t> Configuration::ParseBooleanToken( const String& pValue ) {
    int8_t index= 0;
    Substring value(pValue);
    if( value.Trim().IsEmpty() )
        return {false, int8_t(-1)};

    for ( auto& it : BooleanTokens ) {
        if ( it.first .Match(value)  ) return {false, index};
        if ( it.second.Match(value)  ) return {true , index};
        ++index;
    }
    return {false, int8_t(-1)};
}

AString& Configuration::WriteBooleanToken( bool value, int8_t idxRequested, AString& dest ) {
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

bool Configuration::DeletePath( const String& path ) {
DOX_MARKER( [DOX_VARIABLES_DELETE_SAMPLE] )
// get root node of the tree
Cursor cs= Root();

// try to walk the givedn path. If a remainder of the string exits, exit.
if( cs.GoTo(path).IsNotEmpty() )
    return false;

// now delete the subtree, including the node the cursor represents
cs.Delete();

// that's it
return true;
DOX_MARKER( [DOX_VARIABLES_DELETE_SAMPLE] )
}

//##################################################################################################
// Implementation of parsing methods of built-in record types.
//##################################################################################################
void ERPriority::Parse()
{
    enumrecords::bootstrap::EnumRecordParser::Get( ERSerializable::EnumElementName  );
    enumrecords::bootstrap::EnumRecordParser::Get( Priority                  , true );
}

}} // namespace [alib::variables]


//##################################################################################################
// struct AppendableTraits<Variable>
//##################################################################################################
#if !DOXYGEN

namespace alib::strings {

void AppendableTraits<variables::Variable,nchar, lang::HeapAllocator>::operator()(
               TAString<nchar, lang::HeapAllocator>& target, const variables::Variable& variable ) {
#if ALIB_CHARACTERS_WIDE
    String256 name;
    variable.Name(name);
    target << name;
#else
    variable.Name(target);
#endif

}

void AppendableTraits<variables::Variable,wchar, lang::HeapAllocator>::operator()(
               TAString<wchar, lang::HeapAllocator>& target, const variables::Variable& variable ) {
#if ALIB_CHARACTERS_WIDE
    variable.Name(target);
#else
    String256 name;
    variable.Name(name);
    target << name;
#endif
}


} // namespace [alib::strings]
#endif // DOXYGEN

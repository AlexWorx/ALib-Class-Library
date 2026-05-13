
using namespace alib::system;
namespace alib::filetree {

//==================================================================================================
//=== FTreeNodeHandler
//==================================================================================================
namespace detail {

#   include "ALib.Lang.CIFunctions.H"
void FTreeNodeHandler::AllocateExtendedInfo( FTree::Cursor&      node,
                                             const PathString&   symLinkDest,
                                             const PathString&   symLinkRealPath ) {
    ALIB_ASSERT_ERROR(    node->Type() == FileStatus::Types::DIRECTORY
                       || node->Type() == FileStatus::Types::SYMBOLIC_LINK
                       || node->Type() == FileStatus::Types::SYMBOLIC_LINK_DIR,
                       "FILETREE", "Given node is not a directory or symbolic link." )

    ALIB_ASSERT_ERROR(     (node->Type() == FileStatus::Types::DIRECTORY)
                       ==  symLinkDest.IsEmpty(),
                       "FILETREE", "Error in symbolic link parameter" )


    auto& v = *node;
    bool isAllocated= v.GetExtendedInfo(); // this might happen with forced rescans

    auto pool= node.Tree<FTree>().Pool();
    ALIB_ALLOW_SPARSE_ENUM_SWITCH
    switch (v.Type()) {
        case FileStatus::Types::DIRECTORY:
        {
            if( isAllocated )
                *static_cast<FTValue::EIDirectory*>(v.GetExtendedInfo())= FTValue::EIDirectory();
            else
                v.SetExtendedInfo( pool.New<FTValue::EIDirectory>() );
        }
        return;

        case FileStatus::Types::SYMBOLIC_LINK:
        {
            if( isAllocated )
                *static_cast<FTValue::EISymLinkFile*>(v.GetExtendedInfo())= FTValue::EISymLinkFile();
            else
                v.SetExtendedInfo( pool.New<FTValue::EISymLinkFile>() );
            v.SetLinkTarget( node.Tree<FTree>(), symLinkDest, symLinkRealPath);
        }
        return;

        case FileStatus::Types::SYMBOLIC_LINK_DIR:
        {
            if( isAllocated )
                *static_cast<FTValue::EISymLinkDir*>(v.GetExtendedInfo())= FTValue::EISymLinkDir();
            else
                v.SetExtendedInfo( pool.New<FTValue::EISymLinkDir>() );
            v.SetLinkTarget( node.Tree<FTree>(), symLinkDest, symLinkRealPath);
        }
        return;

        default:
        return;
    }
    ALIB_POP_ALLOWANCE
}
#   include "ALib.Lang.CIMethods.H"

} // namespace alib::filetree[::detail]

//==================================================================================================
//=== FTree
//==================================================================================================
FTree::FTree( MonoAllocator& allocator )
: StringTree( allocator, DIRECTORY_SEPARATOR )
, Pool      ( allocator )
, ogResolver( Pool )
, listeners ( allocator ) {
    ConstructRootValue();
    numberFormat.FractionalPartWidth= 1;

                   DbgSetDCSName("FTree");

    ALIB_DBG(   if( alib::FILETREE.IsBootstrapped())
    {
        Log_SetDomain( "ALIB/FILETREE", Scope::Path)
        Log_SetDomain( "FTREE"     , Scope::Filename)
    }                                                   )
}

FTree::~FTree() {
    #if ALIB_DEBUG
    for( auto& node : nodeTable )
        if( node.data.custom ) {
            Path path;
            createCursor(node).AssemblePath(path);
            ALIB_ERROR( "FILETREE",
            "CustomData not deleted before destruction of class FTree.\n"
            "    First node found: {}.\n"
            "  Attached data type: {}"  , path, node.data.dbgCustomType )
        }
    #endif

    // we have to delete all nodes before the invocation of the base destructor, because
    // this would use our pool allocator on existing nodes (which is then destructed already).
    Clear();

    // delete root value
    auto* extendedInfo= Root()->GetExtendedInfo();
    if( extendedInfo )
        Pool().Delete( static_cast<FTValue::EIDirectory*>(extendedInfo) );
    
    DestructRootValue();
}

void FTree::registerListener( FTreeListener*             listener,
                              lang::ContainerOp          insertOrRemove,
                              FTreeListener::Event       event,
                              const FTFile*              file,
                              const StringTree::Cursor*  subTree,
                              const PathString&          fileName,
                              const PathString&          pathPrefix,
                              const PathString&          pathSubstring  ) {
    // checks
    ALIB_ASSERT_ERROR( file    ==nullptr || &file->AsCursor().Tree() == this,"FILETREE","Given file does not belong to this FTree." )
    ALIB_ASSERT_ERROR( subTree ==nullptr ||  subTree->IsValid()             ,"FILETREE","Invalid cursor given." )
    ALIB_ASSERT_ERROR( subTree ==nullptr || &subTree        ->Tree() == this,"FILETREE","Given cursor does not belong to this FTree." )

  //------------------------------------------ registration ----------------------------------------
    if( insertOrRemove == lang::ContainerOp::Insert) {
        listeners.emplace_back( ListenerRecord{ listener,
                                               event,
                                               (file     ? file->AsCursor().Export() : ConstCursorHandle()),
                                               (subTree  ? subTree->        Export() : ConstCursorHandle()),
                                               PathStringPA(Pool),
                                               PathStringPA(Pool),
                                               PathStringPA(Pool)   } );
        listeners.back().fileName     << fileName;
        listeners.back().pathPrefix   << pathPrefix;
        listeners.back().pathSubstring<< pathSubstring;

        return;
    }

  //----------------------------------------------- de ---------------------------------------------
    for (auto it= listeners.begin() ; it != listeners.end() ; ++it )
        if(     it->listener == listener
            &&  it->event    == event
            &&  it->file     == ( file    ? file->AsCursor().Export() : ConstCursorHandle() )
            &&  it->subTree  == ( subTree ? subTree        ->Export() : ConstCursorHandle() )
            &&  it->fileName     .Equals( fileName )
            &&  it->pathPrefix   .Equals( pathPrefix )
            &&  it->pathSubstring.Equals( pathSubstring )    )
        {
            (void) listeners.erase( it );
            return;
        }

    ALIB_WARNING("FILETREE", "Listener with matching set of parameters not found with deregistration.")

}  // FTree::registerListener


int FTree::MonitorStop( FTreeListener*  listener ) {
    // checks
    ALIB_ASSERT_ERROR( listener!=nullptr, "FILETREE", "Given listener is nullptr." )

  //----------------------------------------------- de ---------------------------------------------
    int cnt= 0;
    for (auto it= listeners.begin() ; it != listeners.end() ; )
        if( it->listener == listener ) {
            Log_Verbose("Removing listener")
            it= listeners.erase( it );
            ++cnt;
        }
        else
             ++it;

    Log_If(cnt==0, Verbosity::Warning, "No listener found to be removed." )
    
    return cnt;
}  // FTree::registerListener

void FTree::notifyListeners(  FTreeListener::Event event,
                              FTFile&              file,
                              const PathString&    filePathGiven  ) {
    Path                filePathBuffer;
    const PathString*   filePath= &filePathGiven;
    for (auto it= listeners.begin() ; it != listeners.end() ; ++it )
        if( event == it->event ) {
            // if needed generate file path
            if(     filePath->IsEmpty()
                &&  (   it->fileName     .IsNotEmpty()
                     || it->pathPrefix   .IsNotEmpty()
                     || it->pathSubstring.IsNotEmpty() )     )
            {
                (file.AsCursor().IsRoot() ? file.AsCursor()
                                          : file.AsCursor().Parent() ).AssemblePath(filePathBuffer);
                filePath= &filePathBuffer;
            }

            if(    ( it->file         .IsValid()    && ( it->file ==  file.AsCursor().Export() ) )
                || ( it->subTree      .IsValid()    && ( file.AsCursor().Distance( ImportCursor(it->subTree) ) >= 0 ) )
                || ( it->fileName     .IsNotEmpty() && it->fileName.Equals(file.AsCursor().Name()) )
                || ( it->pathPrefix   .IsNotEmpty() && filePath->StartsWith(it->pathPrefix) )
                || ( it->pathSubstring.IsNotEmpty() && filePath->IndexOf(it->pathSubstring) >= 0 )
               )
            {
                Log_Verbose("Notifying listener. Event=", event == FTreeListener::Event::CreateNode
                                                          ? "CreateNode" : "DeleteNode" )
                it->listener->Notify( file, event );
    }       }
} // FTree::notifyListeners


#   include "ALib.Lang.CIFunctions.H"
void FTree::FixSums( Cursor directory) {
    ALIB_ASSERT_ERROR( directory->Type() == FileStatus::Types::DIRECTORY,
                       "FILETREE", "Given node is not a directory." )

    FTValue::DirectorySums& sums= directory->Sums();
    sums= FTValue::DirectorySums();
    directory.GoToFirstChild();
    while( directory.IsValid()) {
        FTValue& v= *directory;
        sums.TypeCounters[size_t(v.Type())]++;
        if( v.IsDirectory() )
            sums+= v.Sums();

        directory.GoToNextSibling();
}   }

//==================================================================================================
//=== Debug Dump
//==================================================================================================

#if ALIB_DEBUG && !DOXYGEN

String DBG_DUMP_FORMAT=
    A_CHAR("{:ta h{2,r} on{10,r} gn{10,r} s(IEC){10,r} dm qqq FxFa (rd{3r}' D' rf{3r}' F' re{2r}' EA' rb{2r}'BL) 'nx l b }\n");

AString&     DbgDump( AString&                  target,
                      FTree&                    tree,
                      EnumBitSet<FileStatus::Types>  includedTypes,
                      FTree::Cursor             startNode    ,
                      unsigned                  depth           ) {
    if( startNode.IsInvalid() )
        startNode= tree.Root().AsCursor();

    ALIB_LOCK_RECURSIVE_WITH(Formatter::DEFAULT_LOCK)
    Formatter& fmt= *Formatter::DEFAULT;
    fmt.Reset();
DOX_MARKER( [DOX_FILETREE_ITERATION])
// create and configure iteratator
StringTreeIterator<FTree> stit;
stit.SetPathGeneration(lang::Switch::Off);
stit.SetMaxDepth( depth );
stit.Initialize ( startNode, startNode.IsRoot() ? lang::Inclusion::Exclude
                                                : lang::Inclusion::Include );
// loop over all nodes and dump
while( stit.IsValid()) {
    if( includedTypes.Test(stit.Node()->Type()))
        fmt.Format( target, DBG_DUMP_FORMAT, FTFile(stit.Node())  );
    stit.Next();
}
DOX_MARKER( [DOX_FILETREE_ITERATION])
    return target;
}

#endif // ALIB_DEBUG && !DOXYGEN  (dump methods)
#   include "ALib.Lang.CIMethods.H"

} // namespace alib::filetree

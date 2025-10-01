// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/alox/alox.prepro.hpp"
#include "alib/files/files.prepro.hpp"
#if !defined ( _WIN32 )
#   include <pwd.h>
#   include <grp.h>
#endif


// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Files;
    import   ALib.Lang;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
#  if ALIB_ALOX
    import   ALib.ALox;
    import   ALib.ALox.Impl;
#  endif
#  if ALIB_EXPRESSIONS
    import   ALib.Expressions;
#  endif
#if ALIB_DEBUG
#  include "ALib.Format.H"
#endif
#else
#   include "ALib.Lang.H"
#   include "ALib.Characters.Functions.H"
#   include "ALib.Strings.H"
#   include "ALib.ALox.H"
#   include "ALib.ALox.Impl.H"
#   include "ALib.Expressions.H"
#if ALIB_DEBUG
#  include "ALib.Format.H"
#endif
#   include "ALib.Files.H"
#endif
// ======================================   Implementation   =======================================

using namespace alib::system;
namespace alib::files {

//==================================================================================================
//=== FTreeNodeHandler
//==================================================================================================
namespace detail {

#   include "ALib.Lang.CIFunctions.H"
void FTreeNodeHandler::AllocateExtendedInfo( FTree::Cursor&      node,
                                             const PathString&   symLinkDest,
                                             const PathString&   symLinkRealPath )
{
    ALIB_ASSERT_ERROR(    node->Type() == FInfo::Types::DIRECTORY
                       || node->Type() == FInfo::Types::SYMBOLIC_LINK
                       || node->Type() == FInfo::Types::SYMBOLIC_LINK_DIR,
                       "FILES", "Given node is not a directory or symbolic link." )

    ALIB_ASSERT_ERROR(     (node->Type() == FInfo::Types::DIRECTORY)
                       ==  symLinkDest.IsEmpty(),
                       "FILES", "Error in symbolic link parameter" )


    auto& v = *node;
    ALIB_ASSERT_ERROR( v.GetExtendedInfo() == nullptr, "FILES", "Already set" )

    auto pool= node.Tree<FTree>().Pool();
    ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
    switch (v.Type())
    {
        case FInfo::Types::DIRECTORY:
        {
            v.SetExtendedInfo( pool.New<FInfo::EIDirectory>() );
        }
        return;

        case FInfo::Types::SYMBOLIC_LINK:
        {
            v.SetExtendedInfo( pool.New<FInfo::EISymLinkFile>() );
            v.SetLinkTarget( node.Tree<FTree>(), symLinkDest, symLinkRealPath);
        }
        return;

        case FInfo::Types::SYMBOLIC_LINK_DIR:
        {
            v.SetExtendedInfo( pool.New<FInfo::EISymLinkDir>() );
            v.SetLinkTarget( node.Tree<FTree>(), symLinkDest, symLinkRealPath);
        }
        return;

        default:
        return;
    }
    ALIB_WARNINGS_RESTORE
}
#   include "ALib.Lang.CIMethods.H"

} // namespace alib::files[::detail]

//==================================================================================================
//=== FTree
//==================================================================================================
FTree::FTree( MonoAllocator& allocator )
: StringTree( allocator, DIRECTORY_SEPARATOR )
, Pool      ( allocator )
, ogResolver( Pool )
, listeners ( allocator )
{
    ConstructRootValue();
    numberFormat.FractionalPartWidth= 1;

                   DbgSetDCSName("FTree");

    ALIB_DBG(   if( alib::FILES.IsBootstrapped())
    {
        Log_SetDomain( "ALIB/FILES", Scope::Path)
        Log_SetDomain( "FTREE"     , Scope::Filename)
    }                                                   )
}

FTree::~FTree()
{
    #if ALIB_DEBUG
    for( auto& node : nodeTable )
        if( node.data.custom )
        {
            Path path;
            createCursor(node).AssemblePath(path);
            ALIB_ERROR( "FILES",
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
        Pool().Delete( reinterpret_cast<FInfo::EIDirectory*>(extendedInfo) );
    
    DestructRootValue();
}

void FTree::registerListener( FTreeListener*             listener,
                              lang::ContainerOp          insertOrRemove,
                              FTreeListener::Event       event,
                              const File*                file,
                              const StringTree::Cursor*  subTree,
                              const PathString&          fileName,
                              const PathString&          pathPrefix,
                              const PathString&          pathSubstring  )
{
    // checks
    ALIB_ASSERT_ERROR( file    ==nullptr || &file->AsCursor().Tree() == this,"FILES","Given file does not belong to this FTree." )
    ALIB_ASSERT_ERROR( subTree ==nullptr ||  subTree->IsValid()             ,"FILES","Invalid cursor given." )
    ALIB_ASSERT_ERROR( subTree ==nullptr || &subTree        ->Tree() == this,"FILES","Given cursor does not belong to this FTree." )

    // ---------------- registration ---------------------
    if( insertOrRemove == lang::ContainerOp::Insert)
    {
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

    // ---------------- de-registration ---------------------
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

    ALIB_WARNING("FILES", "Listener with matching set of parameters not found with deregistration.")

}  // FTree::registerListener


int FTree::MonitorStop( FTreeListener*  listener )
{
    // checks
    ALIB_ASSERT_ERROR( listener!=nullptr, "FILES", "Given listener is nullptr." )

    // ---------------- de-registration ---------------------
    int cnt= 0;
    for (auto it= listeners.begin() ; it != listeners.end() ; )
        if( it->listener == listener )
        {
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
                              File&                file
           IF_ALIB_THREADS( , SharedLock*          lock) ,
                              const PathString&    filePathGiven  )
{
    Path                filePathBuffer;
    const PathString*   filePath= &filePathGiven;
    for (auto it= listeners.begin() ; it != listeners.end() ; ++it )
        if( event == it->event )
        {
            // if needed generate file path
            if(     filePath->IsEmpty()
                &&  (   it->fileName     .IsNotEmpty()
                     || it->pathPrefix   .IsNotEmpty()
                     || it->pathSubstring.IsNotEmpty() )     )
            {
IF_ALIB_THREADS( if (lock) lock->AcquireShared(ALIB_CALLER_PRUNED); )
                   (file.AsCursor().IsRoot() ? file.AsCursor()
                                             : file.AsCursor().Parent() )
                       .AssemblePath(filePathBuffer);
IF_ALIB_THREADS( if (lock) lock->ReleaseShared(ALIB_CALLER_PRUNED); )
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
            }
     }
} // FTree::notifyListeners


#   include "ALib.Lang.CIFunctions.H"
void FTree::FixSums( Cursor directory)
{
    ALIB_ASSERT_ERROR( directory->Type() == FInfo::Types::DIRECTORY,
                       "FILES", "Given node is not a directory." )

    FInfo::DirectorySums& sums= directory->Sums();
    sums= FInfo::DirectorySums();
    directory.GoToFirstChild();
    while( directory.IsValid())
    {
        FInfo& v= *directory;
        sums.TypeCounters[size_t(v.Type())]++;
        if( v.IsDirectory() )
            sums+= v.Sums();

        directory.GoToNextSibling();
    }
}

//==================================================================================================
//=== Debug Dump
//==================================================================================================

#if ALIB_DEBUG && !DOXYGEN

String DBG_DUMP_FORMAT=
    A_CHAR("{:ta h{2,r} on{10,r} gn{10,r} s(IEC){10,r} dm qqq FxFa (rd{3r}' D' rf{3r}' F' re{2r}' EA' rb{2r}'BL) 'nf l}\n");

AString&     DbgDump( AString&                  target,
                      FTree&                    tree,
                      EnumBitSet<FInfo::Types>  includedTypes,
                      FTree::Cursor             startNode    ,
                      unsigned int              depth           )

{
    if( startNode.IsInvalid() )
        startNode= tree.Root().AsCursor();

    ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    Formatter& fmt= *Formatter::Default;
    fmt.Reset();
    FTree::RecursiveIterator rit;
    rit.SetPathGeneration(lang::Switch::Off);

    // loop over all nodes and dump
    fmt.Format( target, DBG_DUMP_FORMAT, File(startNode) );

    rit.Initialize( startNode, depth );
    while( rit.IsValid())
    {
        if( includedTypes.Test(rit.Node()->Type()))
            fmt.Format( target, DBG_DUMP_FORMAT, File(rit.Node())     );
        rit.Next();
    }

    return target;
}

#endif // ALIB_DEBUG && !DOXYGEN  (dump methods)
#   include "ALib.Lang.CIMethods.H"

} // namespace alib::files




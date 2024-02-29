// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/files/ftree.hpp"
#include "alib/files/ftools.hpp"

#if ALIB_DEBUG
#   if !defined(HPP_ALIB_LANG_FORMAT_FORMATTER)
#      include "alib/lang/format/formatter.hpp"
#   endif
#endif

#if !defined ( _WIN32 )                          
#   include <pwd.h>
#   include <grp.h>
#endif
namespace alib::files {


//==================================================================================================
//=== FTree
//==================================================================================================
FTree::FTree(monomem::MonoAllocator* allocator)
: StringTree(allocator, DirectorySeparator)
{
    ConstructRootValue();
}

void FTree::FixSums( Cursor node)
{
    ALIB_ASSERT_ERROR( node.Value().Type() == FInfo::Types::DIRECTORY,
                       "CAMP/FILES", "Given node is not a directory.")

    FInfo::DirectorySums& sums= node.Value().Sums();
    sums= FInfo::DirectorySums();
    node.GoToFirstChild();
    while( node.IsValid())
    {
        FInfo& v= node.Value();
        sums.TypeCounters[int(v.Type())]++;
        if( v.IsDirectory() )
            sums+= v.Sums();

        node.GoToNextSibling();
    }
}

void FTree::AllocateExtendedInfo(Cursor& node, String& symLinkDest, String& symLinkRealPath)
{
    ALIB_ASSERT_ERROR(    node.Value().Type() == FInfo::Types::DIRECTORY
                       || node.Value().Type() == FInfo::Types::SYMBOLIC_LINK
                       || node.Value().Type() == FInfo::Types::SYMBOLIC_LINK_DIR,
                       "CAMP/FILES", "Given node is not a directory or symbolic link.")

    ALIB_ASSERT_ERROR(     (node.Value().Type() == FInfo::Types::DIRECTORY)
                       ==  symLinkDest.IsEmpty(),
                       "CAMP/FILES", "Error in symbolic link parameter" )


    auto& v = node.Value();
    auto* ma= GetAllocator();
    ALIB_ASSERT_ERROR( v.GetExtendedInfo() == nullptr, "CAMP/FILES", "Already set")

    ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
    switch (v.Type())
    {
        case FInfo::Types::DIRECTORY:
        {
            if( recyclerEIDir != nullptr )
            {
                auto* recycled= recyclerEIDir;
                recyclerEIDir= recycled->next;
                recycled->data= FInfo::EIDirectory();
                v.SetExtendedInfo(&recycled->data);
                return;
            }

            v.SetExtendedInfo( &ma->Emplace<LinkedEIDir>()->data );
        }
        return;

        case FInfo::Types::SYMBOLIC_LINK:
        {
            if( recyclerEISL != nullptr )
            {
                auto* recycled= recyclerEISL;
                recyclerEISL= recycled->next;
                recycled->data= FInfo::EISymLinkFile();
                v.SetExtendedInfo(&recycled->data);
            }
            else
                v.SetExtendedInfo( &ma->Emplace<LinkedEISL>()->data );
            v.SetLinkTarget(symLinkDest, symLinkRealPath);
        }
        return;

        case FInfo::Types::SYMBOLIC_LINK_DIR:
        {
            if( recyclerEISlDir != nullptr )
            {
                auto* recycled= recyclerEISlDir;
                recyclerEISlDir= recycled->next;
                recycled->data= FInfo::EISymLinkDir();
                v.SetExtendedInfo(&recycled->data);
            }
            else
                v.SetExtendedInfo( &ma->Emplace<LinkedEISLDir>()->data );
            v.SetLinkTarget(symLinkDest, symLinkRealPath);
        }
        return;
        default:
        return;
    }
    ALIB_WARNINGS_RESTORE
}

integer FTree::CountRecyclables( FInfo::Types type )
{
    int result= 0;
    if( type == FInfo::Types::DIRECTORY )
    {
        auto* act= recyclerEIDir;
        while( act )
        {
            ++result;
            act= act->next;
        }
        return result;
    }

    if( type == FInfo::Types::SYMBOLIC_LINK )
    {
        auto* act= recyclerEISL;
        while( act )
        {
            ++result;
            act= act->next;
        }
        return result;
    }

    if( type == FInfo::Types::SYMBOLIC_LINK_DIR )
    {
        auto* act= recyclerEISlDir;
        while( act )
        {
            ++result;
            act= act->next;
        }
        return result;
    }

    ALIB_ERROR( "CAMP/FILES", "No extended information for type {!Q}", type )
    return 0;
}


//==================================================================================================
//=== Debug Dump
//==================================================================================================

#if ALIB_DEBUG && !defined(ALIB_DOX)
namespace {
void dbgDumpEntry( AString&                 buf      ,
                   OwnerAndGroupResolver&   ogResolver,
                   SPFormatter              fmt      ,
                   const int*               nameWidth,
                   const FTree::Cursor&    node       )
{
    String32 bufPerms;

    auto& entry= node.Value();
    std::pair<String,String> ownerAndGroup= ogResolver.Get( entry );

    auto type= entry.Type();

    String128 bufSize;
    uinteger size= entry.Size();
    if( size <1000 )
        fmt->Format( bufSize, "{:5}"  , size );
    else
    {
        uinteger ks= 1024;
        for( const char* entity : {"K", "M", "G", "T" , "P", "E" } )
        {
            if( size < ks*1000 )
            {
                fmt->Format( bufSize, "{:5.1}{}", double(size) / double(ks), entity );
                break;
            }
            ks*= 1024;
        }
    }


    auto quality= entry.Quality();

    String128 bufSizes;
    if(    (     type == FInfo::Types::DIRECTORY
             ||  type == FInfo::Types::SYMBOLIC_LINK_DIR )
        && quality == FInfo::Qualities::RECURSIVE   )
    {
        FInfo::DirectorySums& dirInfo= entry.Sums();
        fmt->Format( bufSizes, "({}d, {}f, {}ea, {}bl)",
                               dirInfo.CountDirectories(),
                               dirInfo.CountNonDirectories(),
                               dirInfo.QtyErrsAccess,
                               dirInfo.QtyErrsBrokenLink           );
    }
    String scanResultText;
    switch (quality)
    {
        case FInfo::Qualities::NONE:                  scanResultText= A_CHAR("---"); break;
        case FInfo::Qualities::STATS:                 entry.IsDirectory() ? scanResultText= A_CHAR("sta")
                                                                          : scanResultText= A_CHAR("OK ");  break;
        case FInfo::Qualities::RESOLVED:              scanResultText= A_CHAR("res"); break;
        case FInfo::Qualities::RECURSIVE:             scanResultText= A_CHAR("OK "); break;

        case FInfo::Qualities::MAX_DEPTH_REACHED:     scanResultText= A_CHAR("MXD"); break;
        case FInfo::Qualities::NOT_FOLLOWED:          scanResultText= A_CHAR("NOF"); break;
        case FInfo::Qualities::NOT_CROSSING_FS:       scanResultText= A_CHAR("NOX"); break;
        case FInfo::Qualities::NO_AFS:                scanResultText= A_CHAR("AFS"); break;

        case FInfo::Qualities::NO_ACCESS:             scanResultText= A_CHAR("NA "); break;
        case FInfo::Qualities::NO_ACCESS_SL:          scanResultText= A_CHAR("NAL"); break;
        case FInfo::Qualities::NO_ACCESS_SL_TARGET:   scanResultText= A_CHAR("NAT"); break;
        case FInfo::Qualities::NO_ACCESS_DIR:         scanResultText= A_CHAR("NAD"); break;
        case FInfo::Qualities::BROKEN_LINK:           scanResultText= A_CHAR("BRL"); break;
        case FInfo::Qualities::CIRCULAR_LINK:         scanResultText= A_CHAR("CIL"); break;
        case FInfo::Qualities::DUPLICATE:             scanResultText= A_CHAR("DBL"); break;
        case FInfo::Qualities::NOT_EXISTENT:          scanResultText= A_CHAR("NEX"); break;
        case FInfo::Qualities::UNKNOWN_ERROR:         scanResultText= A_CHAR("UKN"); break;
    }

    String4K symlinkInfo;
    if(    (    type == FInfo::Types::SYMBOLIC_LINK
             || type == FInfo::Types::SYMBOLIC_LINK_DIR )
        && entry.Quality() >= FInfo::Qualities::RESOLVED )
    {
        symlinkInfo <<  " -> " << entry.GetLinkTarget();
        if(     entry.GetRealLinkTarget().IsNotEmpty()
            && !entry.GetLinkTarget().Equals( entry.GetRealLinkTarget()) )
            symlinkInfo <<  " (" << entry.GetRealLinkTarget() <<  ")";
        if( quality == FInfo::Qualities::BROKEN_LINK )
             symlinkInfo <<" (Broken)";
    }

    // print first part
    fmt->Format( buf, "{} {:>4} {:>4} {:>6} {:yyyy-MM-dd HH:mm} {} {}{}",
                 entry.WriteTypeAndAccess( bufPerms.Reset() ),
                 ownerAndGroup.first, ownerAndGroup.second,
                 bufSize,
                 entry.MTime(),
                 scanResultText,
                 entry.IsCrossingFS()   ? 'M' : '-',
                 entry.IsArtificialFS() ? 'A' : '-'         );

    buf << ' ';

    // print path as table
    {
        // build stack of parent nodes
        auto actNode= node;
        FTree::Cursor nodeStack[256];
        int sp= 1;
        nodeStack[0]= actNode;
        while( actNode.GoToParent().IsValid() )
            nodeStack[sp++]= actNode;

        // process stack reversely
        String8 DirSep(DirectorySeparator);
        for (int i = sp-1; i >= 0; --i)
        {
            const String& name= nodeStack[i].Name();
            fmt->Format( buf, "{}{}{!FillC }",
                         i > sp-3        ? ""     : DirSep,
                         name.IsEmpty()  ? DirSep : name,
                         i>0 ? nameWidth[sp-i-1] - name.Length() : 0);
        }

    }

    buf << ' ';

    // print last part
    fmt->Format( buf, "{} {}\n", symlinkInfo, bufSizes  );


} //dbgDumpEntry()
}// anonymous namespace

AString&     DbgDump( AString&                  target,
                      FTree&                    tree,
                      EnumBitSet<FInfo::Types>  includedTypes,
                      FTree::Cursor             startNode    ,
                      unsigned int              depth           )

{
    if( startNode.IsInvalid() )
        startNode= tree.Root();

    SPFormatter fmt= Formatter::AcquireDefault( ALIB_CALLER_PRUNED );
    FTree::RecursiveIterator rit;
    rit.SetPathGeneration(lang::Switch::Off);

    // determine the maximum width of child names for each depth level
    int nameWidth[256];
    {
        for (int i = 0; i < 256; ++i) nameWidth[i]=0;
        rit.Initialize( startNode );
        while( rit.IsValid())
        {
            if( includedTypes.Test(rit.Node().Value().Type()))
                nameWidth[rit.CurrentDepth()+1]= (std::max)( nameWidth[rit.CurrentDepth()+1],
                                                             int(rit.Node().Name().Length())    );
            rit.Next();
        }
    }

    // loop over all nodes and dump
    OwnerAndGroupResolver ogResolver;
    dbgDumpEntry( target, ogResolver, fmt, nameWidth, startNode );
    rit.Initialize( startNode, depth );
    while( rit.IsValid())
    {
        if( includedTypes.Test(rit.Node().Value().Type()))
            dbgDumpEntry( target, ogResolver, fmt, nameWidth, rit.Node() );
        rit.Next();
    }

    fmt->Release();

    return target;
}

#endif // ALIB_DEBUG && !defined(ALIB_DOX)  (dump methods)

} // namespace alib::files

// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/config/inifile.hpp"
#   include "alib/config/configcamp.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"
#   include "alib/lang/message/exception.hpp"
#   include "alib/strings/util/tokenizer.hpp"
#   include "alib/compatibility/std_strings_iostream.hpp"
#   include <fstream>
#endif // !DOXYGEN

using namespace alib::lang::system;

namespace alib {  namespace config {

// #################################################################################################
//  helpers
// #################################################################################################
bool IniFile::startsWithCommentSymbol( String& subs )
{
    auto c= subs.CharAtStart();
    return       c == '#'
            ||   c == ';'
            || ( c == '/' && subs.Length() > 1 && subs.CharAt(1) == '/'  );
}



// #################################################################################################
// Constructor/Destructor
// #################################################################################################
IniFile::IniFile()
: Allocator ( ALIB_DBG("IniFile",) 8 )
, entryTable( Allocator )
, Sections  ( Allocator )
, LinesWithReadErrors( Allocator )
{}

void                IniFile::Reset()
{
    FileComments       = nullptr;
    FileName           = nullptr;
    entryTable         .Reset();
    Sections           .Reset();
    LinesWithReadErrors.Reset();
    Allocator          .Reset();
}

IniFile::Section*  IniFile::CreateSection( const String& name )
{
    auto& section= Sections.EmplaceBack(Allocator);
    section.Name.Allocate(Allocator, name );
    return &section;
}

IniFile::Section*  IniFile::DeleteSection( const String& name )
{
    // search section
    for(auto secIt= Sections.begin() ; secIt != Sections.end(); ++secIt )
    {
        // found?
        if( secIt->Name.Equals(name))
        {
            auto* section= &*secIt;
            // delete hashtable entries
            for(auto entryIt= entryTable.begin() ; entryIt != entryTable.end(); ++entryIt )
            {
                if( entryIt->second.first == section )
                {
                    entryIt= entryTable.Erase( entryIt );
                    continue;
                }
            }

            (void) Sections.Erase(secIt);
            return section;
        }
    }
    return nullptr;
}

IniFile::Entry*    IniFile::DeleteEntry ( Section* section, const String& name )
{
    // search entry
    for(auto entryIt= section->Entries.begin() ; entryIt != section->Entries.end(); ++entryIt )
    {
        // found?
        if( entryIt->Name.Equals(name))
        {
            auto* entry= &*entryIt;
            ALIB_ASSERT_RESULT_GREATER_THAN( entryTable.Erase( EntryKey(section->Name, name)), 0 )
            return entry;
        }
    }
    return nullptr;
}

IniFile::Entry*    IniFile::CreateEntry ( Section* section, const String& pName )
{
    ALIB_ASSERT_ERROR( pName.IsNotEmpty(), "CONFIG", "Empty Ini-File variable name given.")
    Entry& newEntry= section->Entries.PushBack(Entry());
    newEntry.Name.Allocate(Allocator, pName);
    entryTable.EmplaceUnique( EntryKey(section->Name, newEntry.Name),
                              std::make_pair(section, &newEntry) );
    return &newEntry;
}

std::pair<IniFile::Section*, IniFile::Entry*>   IniFile::SearchEntry (const String& sectionName, const String& name )
{
    ALIB_ASSERT_ERROR( sectionName.IsNotNull(), "CONFIG", "Nulled section name given.")
    auto it=  entryTable.Find( EntryKey(sectionName, name ) );
    if( it != entryTable.end() )
        return it.Mapped();
    return std::pair<Section*, Entry*>(nullptr, nullptr);
}

IniFile::Section*  IniFile::SearchSection( const String& sectionName )
{
    ALIB_ASSERT_ERROR(sectionName.IsNotNull(), "CONFIG", "Nulled section name given.")
    auto sIt= Sections.begin();
    while( sIt != Sections.end() )
    {
        if( sIt->Name.Equals<CHK, lang::Case::Ignore>( sectionName ) )
            return &*sIt;
        ++sIt;
    }
    return nullptr;
}

std::pair<IniFile::Section*, bool>
                    IniFile::SearchOrCreateSection( const String& sectionName )
{
    Section* s= SearchSection( sectionName );
    if ( s != nullptr )
        return std::make_pair( s, false );

    return std::make_pair( CreateSection( sectionName ), true );
}

void  IniFile::AddComments ( String& dest, const String& comments, const String& prefix )
{
    String2K buf;
    Tokenizer tknzr( comments, '\n' );
    while( tknzr.HasNext() )
    {
        auto tok= tknzr.Next().TrimEnd();
        tok.ConsumeCharFromEnd('\r');
        if( !startsWithCommentSymbol(tok) )
            buf << prefix;
        buf << tok << NEW_LINE;
    }
    dest.Allocate(Allocator, buf);
}

integer  IniFile::Read(const CPathString&  path)
{
    FileName.Allocate(Allocator, path);

    //-------------  open file -------------
    ALIB_STRINGS_TO_NARROW(path, nPath, 256)
    std::ifstream file( nPath );

    if ( !file.is_open() )
    {
        int errNo= errno;

        // file does not exist ?
        if ( errNo == UnderlyingIntegral(SystemErrors::enoent) )
            return -1;

        // other errors: throw
        Exception e=   CreateSystemException( ALIB_CALLER_NULLED, errNo );
                  e.Add( ALIB_CALLER_NULLED,  config::Exceptions::ErrorOpeningFile,
                         A_CHAR("INI-"), path                                        );
        throw e;
    }


    if( Sections.IsEmpty() )
        CreateSection( EMPTY_STRING  );

    String256   actLine;      ALIB_DBG( actLine    .DbgDisableBufferReplacementWarning(); )
    String4K    actComments;  ALIB_DBG( actComments.DbgDisableBufferReplacementWarning(); )
    Section*    actSection=   &Sections.Front();

    String8     equalSignOrWhitespace('=');
                equalSignOrWhitespace << DEFAULT_WHITESPACES;

    StringReader reader;
    reader.SetStream( &file );

    auto writeBackAttribute= alib::CONFIG.GetResource("INIWB");


    //-------------  read loop -------------
    bool        writebackFlag   = false;
    int         qtyEntriesRead  = 0;
    int         lineNo          = 0;
    bool        fileHeaderRead  = false;
    while( !reader.IsEOF() )
    {
        reader.Read( actLine );  ++lineNo;

        Substring lineTrimmed( actLine );
        lineTrimmed.Trim();

        // end of file header?
        if ( !fileHeaderRead && lineTrimmed.IsEmpty() )
        {
            fileHeaderRead= true;
            FileComments.Allocate(Allocator, actComments);
            actComments.Reset();
        }

        // continued comment or empty section?
        if ( lineTrimmed.IsEmpty() || startsWithCommentSymbol( lineTrimmed ) )
        {
            actComments._(actLine).NewLine();
            continue;
        }

        // writeback flag?
        if( lineTrimmed.Equals<NC,lang::Case::Ignore>(writeBackAttribute) )
        {
            writebackFlag= true;
            continue;
        }

        // section line?
        if ( lineTrimmed.ConsumeChar('[') )
        {
            fileHeaderRead= true;

            if( !lineTrimmed.ConsumeCharFromEnd( ']' ) )
            {
                LinesWithReadErrors.EmplaceBack( lineNo );
                // continue; // we tolerate missing section ends, but still mark this as a read error
            }
            lineTrimmed.TrimEnd();
            actSection= SearchOrCreateSection( lineTrimmed ).first;
            if( actSection->Comments.IsEmpty())
                actSection->Comments.Allocate(Allocator, actComments);
            actSection->WriteBack= writebackFlag;
            writebackFlag= false;

            actComments.Reset();
            continue;
        }

        // Variable line(s)
        {
            String128  actName;
            String4K   actRawValue;

            integer idx= lineTrimmed.IndexOfAny<lang::Inclusion::Include>( equalSignOrWhitespace );
            if( idx < 0 )
            {
                actName << lineTrimmed;
                lineTrimmed.Clear();
            }
            else
            {
                actName << lineTrimmed.Substring( 0, idx );
                actName.TrimEnd();
                lineTrimmed.ConsumeChars( idx );
                actRawValue._(lineTrimmed);
            }

            // read continues as long as lines end with '\' (must not be '\\')
            while (     lineTrimmed.CharAtEnd()  == '\\'
                    && (lineTrimmed.Length() == 1 || lineTrimmed.CharAt<NC>( lineTrimmed.Length() -2 ) != '\\' ) )
            {
                actRawValue.NewLine();
                reader.Read( actLine );
                if ( reader.IsEOF() )
                {
                    // last line of the file ended with '\' !
                    lineTrimmed.Clear();
                    break;
                }
                actLine.TrimEnd();
                actRawValue << (actLine);
                lineTrimmed= actLine;
            }

            // insert entry with raw and trimmed value
            {
                auto* entry= SearchEntry( actSection->Name, actName ).second;
                if( entry == nullptr)
                {
                    entry=   CreateEntry( actSection, actName );
                    ++qtyEntriesRead;
                }
                else
                {
                    ALIB_WARNING("CONFIG", "Variable was found twice in INI-file. First value will be discarded\n"
                                           "on writing. Variable name: ", actName )
                }
                entry->Comments.Allocate(Allocator, actComments );
                entry->RawValue.Allocate(Allocator, actRawValue );
                entry->WriteBack= writebackFlag;
                writebackFlag= false;

                // parse trimmed value
                String4K   trimmedValue;
                Substring  parser= actRawValue;
                parser.Trim().ConsumeChar('=');
                parser.TrimStart();
                Tokenizer tknzr(parser, '\n', true);
                while((parser= tknzr.Next()).IsNotNull())
                {
                    parser.ConsumeCharFromEnd('\r');
                    if(    parser.CharAtEnd() == '\\'
                        && parser.CharAt(parser.Length()-2) != '\\' )
                        parser.ConsumeCharFromEnd<NC>();
                    parser.TrimEnd();
                    if(!startsWithCommentSymbol(parser))
                        trimmedValue << parser;
                }
                entry->Value.Allocate(Allocator, trimmedValue );
            }
            
        } // variable line(s)

        actComments.Reset();
    } // !EOF
    file.close();

    return qtyEntriesRead;
}

void IniFile::Write(const PathString&  pPath)
{
    auto writeBackAttribute= alib::CONFIG.GetResource("INIWB");

    NString256 path(pPath);
    if( path.IsEmpty() )
        path << FileName;
    ALIB_ASSERT_ERROR( path.IsNotEmpty(), "CONFIG",
           "Given Path is empty and no known filename from previous Read() operation available.")

    // open output file
    std::ofstream outputFileStream( path.Terminate(), std::ios::binary );
    if ( !outputFileStream.is_open() )
    {
        int errNo= errno;
        Exception e=  CreateSystemException( ALIB_CALLER_NULLED, errNo );
                  e.Add( ALIB_CALLER_NULLED, config::Exceptions::ErrorWritingFile, "INI-", FileName );
        throw e;
    }

    StringWriter writer;
    writer.SetStream( &outputFileStream );

    // write file header
    if ( FileComments.IsNotEmpty() )
    {
        writer.Write( FileComments );
        if (FileComments.CharAtEnd() != '\n')
            writer.Write( NEW_LINE );
    }

    // loop over all sections
    for ( Section& section : Sections )
    {
        if( section.Name.IsNotEmpty())
        {
            // write section comments, write back flag and name
            if(section.Comments.CharAtStart() !='\n' && section.Comments.CharAtStart() !='\r')
                writer.Write( NEW_LINE );
            writer.Write( section.Comments );
            if(section.WriteBack)
            {
                writer.Write( writeBackAttribute );
                writer.Write( NEW_LINE );
            }
            writer.Write( NString256() << '[' << section.Name << ']' << NEW_LINE );
        }
        
        // variables
        integer maxVarLength= 0;
        for ( auto& entry : section.Entries )
            maxVarLength= (std::max)( maxVarLength, entry.Name.Length() );

        for ( auto& entry : section.Entries )
        {
            // write comments, write-back-flag, name
            if( entry.Comments.IsNotEmpty())
            {
                if (entry.Comments.CharAtStart() != '\n' && entry.Comments.CharAtStart() != '\r')
                    writer.Write( NEW_LINE );
                writer.Write( entry.Comments );
            }
            
            if(entry.WriteBack)
            {
                writer.Write( writeBackAttribute );
                writer.Write( NEW_LINE );
            }
            writer.Write( entry.Name );

            // write value
            if( entry.NewValue.IsNull())
            {
                writer.Write( entry.RawValue );
                if(!entry.NewValue.EndsWith(NEW_LINE))
                    writer.Write( NEW_LINE );
            }
            else
            {
                writer.Write( "=" );
                int cntLine= 0;
                integer maxValLength= 0;
                Substring rest= entry.NewValue;
                for(;;)
                {
                    // write spaces
                    writer.WriteChars(' ', maxVarLength - ( cntLine == 0 ? entry.Name.Length() - 1
                                                                          : -2                   ));

                    Substring actual= rest.ConsumeToken( '\n' );
                    actual.ConsumeCharFromEnd('\r'); // in win case

                    // escape a trailing comment symbol
                    if( actual.CharAtStart() == '#' || actual.CharAtStart() == ';' )
                        writer.Write( "\\" );

                    writer.Write( actual );
                    if( rest.IsEmpty())
                    {
                        writer.Write( NEW_LINE );
                        break;
                    }
                    
                    if( actual.Length() > maxValLength )
                        maxValLength= actual.Length() + 2;
                    writer.WriteChars(' ', maxValLength - actual.Length()  );

                    writer.Write( "\\" );
                    writer.Write( NEW_LINE );

                    ++cntLine;
                }
            }
        }
    }

    // close file
    outputFileStream.close();
}


}} // namespace [alib::config]

// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_CONFIG_INI_FILE)
#   include "alib/config/inifile.hpp"
#endif


#if !defined(HPP_ALIB_CONFIG_CONFIG)
#   include "alib/config/config.hpp"
#endif
#if !defined(HPP_ALIB_CONFIG_CONFIGURATION)
#   include "alib/config/configuration.hpp"
#endif

#if !defined (HPP_ALIB_CAMP_DIRECTORY)
#   include "alib/lang/system/directory.hpp"
#endif
#if !defined (HPP_ALIB_CAMP_PROCESSINFO)
#   include "alib/lang/system/processinfo.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#   include "alib/strings/util/tokenizer.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_UTIL_SPACES)
#   include "alib/strings/util/spaces.hpp"
#endif
#if !defined (HPP_ALIB_LANG_FORMAT_PARAGRAPHS)
#   include "alib/lang/format/paragraphs.hpp"
#endif
#if !defined (HPP_ALIB_COMPATIBILITY_STD_STRINGS_IOSTREAM)
#   include "alib/compatibility/std_strings_iostream.hpp"
#endif

#if !defined (_GLIBCXX_FSTREAM) && !defined(_FSTREAM_)
#   include <fstream>
#endif
#if defined(_MSC_VER) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif
#endif // !defined(ALIB_DOX)


namespace alib {  namespace config {

// #################################################################################################
// static/globals
// #################################################################################################
String                IniFile::DefaultFileExtension                                = A_CHAR(".ini");

// #################################################################################################
// class InMemoryPlugin::Entry
// #################################################################################################
void IniFile::ToVariable( Entry& entry, Variable& variable )                                   const
{
    // if this had been done before, use base method
    if ( entry.Value.IsNotEmpty() )
    {
        InMemoryPlugin::ToVariable( entry, variable );
        return;
    }

    // store delim and comment
    entry.Delim= variable.Delim();
    variable.ReplaceComments( entry.Comments );

    // parse the INI file content

    //-----  remove INI-File specific from raw value -----
    String512 raw;
    raw.DbgDisableBufferReplacementWarning();
    raw._( entry.RawValue );

    // remove '='
    raw.TrimStart();
    if ( raw.CharAtStart() == '=' )
        raw.DeleteStart(1).TrimStart();
    else
    {
        ALIB_WARNING( "CONFIG", "No equal sign in INI-file variable {!Q}.", variable.Fullname() )
    }

    // remove "\\n"
    integer startIdx= 0;
    while ( (startIdx= raw.IndexOf( '\n', startIdx )) >= 0 )
    {
        // find \\n and trim around this location
        int delLen= 2;
        if ( raw.CharAt( --startIdx) == '\r' )
        {
            delLen= 3;
            --startIdx;
        }
        ALIB_ASSERT( raw.CharAt(startIdx) == '\\' )
        raw.Delete( startIdx, delLen );

        startIdx= raw.TrimAt( startIdx );
        if( startIdx >= raw.Length() )
            break;

        // remove endquote of first line and start quote of second
        if( startIdx >0 && raw[startIdx-1] == '"' && raw[startIdx] == '"' )
        {
            --startIdx;
            raw.Delete( startIdx, 2);
        }


        // if now the next value is starting with a comment symbol, we remove to the next \n
        for(;;)
        {
            character c= raw.CharAt( startIdx );
            if (     c != '#'
                &&   c != ';'
                && ( c != '/' || raw.CharAt( startIdx + 1 ) != '/' ) )
                break;

            integer idx= raw.IndexOf( '\n', startIdx );
            if (idx < 0 ) idx= raw.Length();
            raw.Delete( startIdx, idx - startIdx + 1 );
            if( startIdx >= raw.Length() )
                break;
            startIdx= raw.TrimAt( startIdx );
        }
    }

    // now convert
    StringConverter->LoadFromString( variable, raw );

    // copy the parsed values back to our entry
    auto varSize= variable.Size();
    if( varSize > 0 )
    {
        auto valueIt= entry.SetValueCount( varSize );
        entry.Value.Reset( variable.GetString(0) );
        int idx = 1;
        while( valueIt != entry.AdditionalValues.end() )
        {
            valueIt->Reset( variable.GetString( idx++ ) );
            ++valueIt;
        }
    }
}

void IniFile::FromVariable( Entry& entry, Variable& variable )                                 const
{
    entry.RawValue.Reset();
    InMemoryPlugin::FromVariable( entry, variable );
}


// #################################################################################################
// Constructor/Destructor
// #################################################################################################
IniFile::IniFile( const String& fileName )
: InMemoryPlugin     ( NullString() )
, LinesWithReadErrors( allocator    )
{
    // don't read anything
    if ( fileName.StartsWith( A_CHAR("*") ) )
    {
        AutoSave= false;
        return;
    }

    if (     fileName.CharAtStart() != '/'
         &&  fileName.CharAtStart() != '.'
      #if defined(_WIN32)
         && fileName.CharAtStart() != '\\'
         && (fileName.Length() < 2 || fileName.CharAt(1) != ':' )
      #endif
       )
    {
        Directory dir( Directory::SpecialFolder::HomeConfig );
        FileName._( dir.Path      )
                ._( DirectorySeparator );
    }

    if ( fileName.IsEmpty()  )
    {
        FileName._( ProcessInfo::Current().Name  );

        #if defined(_WIN32)
            if( FileName.EndsWith( A_CHAR(".exe") ) )
                FileName.DeleteEnd( 4 );
        #endif

        FileName._( IniFile::DefaultFileExtension );
    }
    else
        FileName <<  fileName;

    ReadFile();

}

void  IniFile::Clear()
{
    InMemoryPlugin::Clear();
    FileComments.Reset();
    LinesWithReadErrors.Clear();
}


// #################################################################################################
// Read/Write files
// #################################################################################################

//! @cond NO_DOX
bool startsWithCommentSymbol( Substring& subs );
bool startsWithCommentSymbol( Substring& subs )
{
    integer i= String(A_CHAR("#;/")).IndexOfOrLength( subs.CharAtStart() );
    return       i <  2
            || ( i == 2 && subs.Length() > 1 && subs[1] == '/'  );
}
//! @endcond

void  IniFile::ReadFile()
{
    Clear();
    createSection( EmptyString()  );

    // open file
    ALIB_STRINGS_TO_NARROW(FileName, nFileName, 1024)
    std::ifstream file( nFileName );


    if ( !file.is_open() )
    {
        int errNo= errno;

        // file does not exist ?
        if ( errNo == UnderlyingIntegral(SystemErrors::enoent) )
            return;

        // other errors: throw
        Exception e=   lang::system::CreateSystemException( ALIB_CALLER_NULLED, errNo );
                  e.Add( ALIB_CALLER_NULLED,  config::Exceptions::ErrorOpeningFile,
                         A_CHAR("INI-"), FileName                                        );
        throw e;
    }

    AString     lineAS;
    String128   name;
    AString     value;
    AString     comments;
    Section*    actSection= &sections.Front();

    int         lineNo= 0;
    bool        fileHeaderRead= false;

    StringReader reader;
    reader.SetStream( &file );

    String16 separatorCharacters( '=' );
    separatorCharacters._( DefaultWhitespaces() );

    while( !reader.IsEOF() )
    {
        reader.Read( lineAS );
        ++lineNo;
        Substring line( lineAS );

        bool isEmpty=       line.Trim().IsEmpty();
        bool isCommentLine= startsWithCommentSymbol( line );

        if ( isCommentLine )
        {
            if ( comments.IsNotEmpty() )
                comments.NewLine();
            comments._(line);
            continue;
        }

        // still processing file header?
        if ( !fileHeaderRead )
        {
            fileHeaderRead= true;
            FileComments=   comments;
            comments.Reset();
        }

        // empty line?
        if ( isEmpty )
        {
            if ( comments.IsNotEmpty() )
                comments.NewLine();
            continue;
        }

        // section line
        if ( line.ConsumeChar('[') )
        {
            fileHeaderRead= true;

            if( !line.ConsumeCharFromEnd( ']' ) )
                LinesWithReadErrors.EmplaceBack( lineNo );

            actSection= SearchOrCreateSection( line ).first;
            actSection->Comments << comments;

            comments.Reset();
            continue;
        }

        // Variable line
        value.Reset();
        integer idx= line.IndexOfAny<lang::Inclusion::Include>( separatorCharacters );
        if( idx < 0 )
        {
            name.Reset( line );
            line.Clear();
        }
        else
        {
            name.Reset()._( line, 0, idx );
            line.ConsumeChars( idx );
            value._(line);
        }

        // read continues as long as lines end with '\' (must not be '\\')
        while (     line.CharAtEnd()  == '\\'
                && (line.Length() == 1 || line.CharAt<false>( line.Length() -2 ) != '\\' ) )
        {
            value.NewLine();
            reader.Read( lineAS );
            if ( reader.IsEOF() )
            {
                // last line of the file ended with '\' !
                line.Clear();
                break;
            }
            lineAS.TrimEnd();
            line= lineAS;

            value._( lineAS );
        }

        // insert entry with raw value
        {
            auto* entry= searchEntry( actSection->Name(), name );
            if( entry == nullptr)
                entry=   createEntry( actSection, name );
            entry->Value           .Reset();
            entry->AdditionalValues.Reset();
            entry->Comments        .Reset( comments );
            entry->RawValue        .Reset( value    );
        }

        comments.Reset();

    }
    file.close();
}

void  IniFile::writeComments( StringWriter& writer, const AString& comments )
{
    // is empty when trimmed?
    if ( Substring(comments).Trim().IsEmpty() )
        return;

    // tokenize by NEWLINE character
    Tokenizer tknzr( comments, '\n' );
    tknzr.TrimChars= A_CHAR(" \r\t"); // \n is not a whitespace

    while( tknzr.Next(lang::Whitespaces::Keep).IsNotNull() )
    {
        if ( !startsWithCommentSymbol( tknzr.Actual ) )
            writer.Write( DefaultCommentPrefix );
        writer.Write( tknzr.Actual );
        *writer.GetStream() << std::endl;
    }

    tknzr.TrimChars=  DefaultWhitespaces();
}

//! @cond NO_DOX
namespace {
    int getAssignmentPos( const AString& value, const String& alignmentSeparator )
    {
        integer idx= value.IndexOf( alignmentSeparator );
        if( idx > 0 )
        {
            integer idxQuote= value.IndexOfOrLength( '"' );
            if (  idxQuote > idx )
                return static_cast<int>(idx);
        }
        return -1;
    }
}
//! @endcond

void IniFile::WriteFile()
{
    // read all variables
    ALIB_STRINGS_TO_NARROW(FileName,nFileName, 1024)
    std::ofstream outputFileStream ( nFileName );
    if ( !outputFileStream.is_open() )
    {
        int errNo= errno;
        Exception e=  lang::system::CreateSystemException( ALIB_CALLER_NULLED, errNo );
                  e.Add( ALIB_CALLER_NULLED, config::Exceptions::ErrorWritingFile, "INI-", FileName );
        throw e;
    }

    StringWriter writer;
    writer.SetStream( &outputFileStream );

    // write file header
    if ( FileComments.IsNotEmpty() )
    {
        writeComments( writer, FileComments );
        outputFileStream << std::endl;
    }

    // loop over all sections
    int cntVars= 0;
    for ( InMemoryPlugin::Section& section : sections )
    {
        // comments, name
        if ( cntVars > 0 )
            outputFileStream << std::endl;

        // write section comments and name
        writeComments( writer, section.Comments );
        if ( section.Name().IsNotEmpty() )
            writer.Write( NString256() << '[' << section.Name() << ']' << NewLine() );

        // variables
        integer maxVarLength= 0;
        for ( auto& entry : section.Entries() )
            maxVarLength= (std::max)( maxVarLength, entry.Name().Length() );

        bool previousVarHasComments= true;
        for ( auto& entry : section.Entries() )
        {
            ++cntVars;

            // write comments
            if( entry.Comments.IsNotEmpty() )
            {
                // we make an extra empty line if previous var had no comments
                if( !previousVarHasComments)
                    outputFileStream << std::endl;

                writeComments(writer, entry.Comments );
            }

            // write name =
            writer.Write( entry.Name() );

            // either write raw value (if it was not changed by the application, which
            // clears the original value)
            if ( entry.RawValue.IsNotEmpty() )
                writer.Write( entry.RawValue );

            // or write the values parsed by the software
            else
            {
                outputFileStream << '=';
                Spaces::Write( outputFileStream, maxVarLength - entry.Name().Length() + 1 );

                String256 externalizedValue;
                externalizedValue.DbgDisableBufferReplacementWarning();
                auto qtyValues= entry.AdditionalValues.Size() + 1;
                auto entryIt  = entry.AdditionalValues.begin();

                //-------- write as single-line ----------
                if ( !HasBits( entry.FmtHints, FormatHints::MultiLine  ) )
                {
                    bool delimSpaces=  (! HasBits( entry.FmtHints, FormatHints::NoDelimSpaces ) );

                    for ( integer idx= 0; idx < qtyValues; ++idx )
                    {
                        const String32* value;
                        if ( idx == 0 )
                            value= &entry.Value;

                        else
                        {
                            value= &*entryIt;
                            ++entryIt;

                            ALIB_ASSERT_ERROR( entry.Delim != 0, "CONFIG",
                                               "No delimiter given for multi-value variable {!Q}.",
                                               entry.Name() )

                            if( delimSpaces && FormatSpaceBeforeDelim)
                                outputFileStream << ' ';

                            writer.Write( NString16() << entry.Delim );

                            if( delimSpaces && FormatSpaceAfterDelim)
                                outputFileStream << ' ';
                        }

                        // externalize value
                        Substring src( *value );
                        externalizedValue.Reset();
                        StringConverter->ExternalizeValue( src, externalizedValue, entry.Delim );
                        writer.Write( externalizedValue );
                    }
                }

                // ---------- write as multi-line ----------
                else
                {
                    integer    backSlashPos= 0;
                    integer    lastLineLen=  0;

                    // Get maximum position of attribute assignment char '=' or ':' (if exists)
                    int  maxAttributeAssignPos     = 0;
                    bool allAttrHavePrecedingBlanks= true;
                    if (entry.FormatAttrAlignment.IsNotEmpty() )
                    {
                        auto entryIt2  = entry.AdditionalValues.begin();
                        for ( integer idx= 0; idx < qtyValues; ++idx )
                        {
                            const String32* value;
                            if ( idx == 0 )
                                value= &entry.Value;

                            // write delim and backslash of previous line, newline and then spaces of actual line
                            else
                            {
                                value= &*entryIt2;
                                ++entryIt2;
                            }

                            int attributeAssignPos= getAssignmentPos( *value, entry.FormatAttrAlignment );
                            if ( attributeAssignPos > 0 )
                            {
                                if ( maxAttributeAssignPos < attributeAssignPos )
                                    maxAttributeAssignPos= attributeAssignPos;
                                allAttrHavePrecedingBlanks&= value->CharAt( attributeAssignPos - 1 ) == ' ';
                            }
                        }
                        if ( !allAttrHavePrecedingBlanks )
                            maxAttributeAssignPos += 1;
                    }

                    // loop over values of entry
                    for ( integer idx= 0; idx < qtyValues; ++idx )
                    {
                        const String32* value;
                        if ( idx == 0 )
                            value= &entry.Value;

                        // write delim and backslash of previous line, newline and then spaces of actual line
                        else
                        {
                            value= &*entryIt;
                            ++entryIt;
                            ALIB_ASSERT_ERROR( entry.Delim != 0, "CONFIG",
                                               "No delimiter given for multi-value variable {!Q}.",
                                               entry.Name() )
                            writer.Write( String8() << entry.Delim );
                            ++lastLineLen;

                            if ( backSlashPos < lastLineLen + 1 )
                                 backSlashPos=  lastLineLen + 4;

                            Spaces::Write( outputFileStream, backSlashPos - lastLineLen );

                            outputFileStream << '\\' << std::endl;

                            Spaces::Write( outputFileStream, maxVarLength + 2 ); // 2 for "= "
                        }

                        // externalize value
                        Substring src( *value );
                        externalizedValue.Reset();
                        StringConverter->ExternalizeValue( src, externalizedValue, entry.Delim );

                        // if first character is a INI comment char, then escape it
                        character firstChar= externalizedValue.CharAt(0);
                        if( idx != 0 && (firstChar == '#' || firstChar == ';' ) )
                            externalizedValue.InsertAt(A_CHAR("\\"), 0 );

                        // if assignment, insert spaces to align assignments
                        if (entry.FormatAttrAlignment.IsNotEmpty() )
                        {
                            int attributeAssignPos= getAssignmentPos( externalizedValue, entry.FormatAttrAlignment );
                            if ( attributeAssignPos > 0 && attributeAssignPos < maxAttributeAssignPos )
                                externalizedValue.InsertChars( ' ',
                                                               maxAttributeAssignPos-attributeAssignPos,
                                                               attributeAssignPos + (FormatIncludeDelimInAttrAlignment ?
                                                                                      0 : entry.FormatAttrAlignment.Length() )
                                                              );
                        }
                        writer.Write( externalizedValue );

                        lastLineLen=  maxVarLength + 2  + externalizedValue.Length();
                    }
                }
            }
            outputFileStream << std::endl;

            // add an empty line if we have comments
            if( (previousVarHasComments= entry.Comments.IsNotEmpty() ) == true )
                outputFileStream << std::endl;
        }
    }

    // close file
    outputFileStream.close();
}

void IniFile::AddResourcedSectionComments( Configuration&   config,
                                           ResourcePool&    resourcePool,
                                           const NString&   resourceCategory,
                                           const NString&   resourceNamePrefix    )
{
    // add section comments from resources to INI-file
    for ( integer i= 0; i < config.CountPlugins(); ++i)
    {
        IniFile* iniFile= dynamic_cast<IniFile*>( config.GetPlugin( i ) );

        if( !iniFile )
            continue;
        for( auto& section : iniFile->sections )
            if(  section.Comments.IsNull() )
            {
                auto& comment=  resourcePool.Get( resourceCategory,
                                                   NString128() << resourceNamePrefix << section.Name()
                                                   ALIB_DBG(, false));
                if( comment.IsNull() )
                    continue;

                Paragraphs text;
                text.LineWidth= iniFile->LineWidth;
                text.AddMarked( comment );
                text.RemoveLastNewLine();
                section.Comments << text.Buffer;
            }
    }

}

}} // namespace [alib::config]

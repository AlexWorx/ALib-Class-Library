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
#include "alib/files/files.prepro.hpp"
#if !defined ( _WIN32 )
#   include <pwd.h>
#   include <grp.h>
#endif
#include "ALib.Compatibility.StdStrings.H"

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Files;
    import   ALib.Lang;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
    import   ALib.Strings.Calendar;
#  if ALIB_EXPRESSIONS
    import   ALib.Expressions;
#  endif
#  if ALIB_DEBUG
#       include "ALib.Format.H"
#  endif
#else
#   include "ALib.Lang.H"
#   include "ALib.Characters.Functions.H"
#   include "ALib.Strings.H"
#   include "ALib.Strings.Calendar.H"
#   include "ALib.Expressions.H"
#   if ALIB_DEBUG
#     include "ALib.Format.H"
#   endif
#   include "ALib.Files.H"
#endif
// ======================================   Implementation   =======================================
using namespace alib::system;
namespace alib::files {

AString&  File::FormatAccessRights(AString& target)                                            const
{
    int perms= int(Value().Perms());
    // This is a "hard-coded" optimized approach. It only works unless the values of enum
    // Permissions are as they have been since decades...
    static_assert(  int(FInfo::Permissions::OWNER_READ  ) ==  0400 &&
                    int(FInfo::Permissions::OWNER_WRITE ) ==  0200 &&
                    int(FInfo::Permissions::OWNER_EXEC  ) ==  0100 &&
                    int(FInfo::Permissions::GROUP_READ  ) ==   040 &&
                    int(FInfo::Permissions::GROUP_WRITE ) ==   020 &&
                    int(FInfo::Permissions::GROUP_EXEC  ) ==   010 &&
                    int(FInfo::Permissions::OTHERS_READ ) ==    04 &&
                    int(FInfo::Permissions::OTHERS_WRITE) ==    02 &&
                    int(FInfo::Permissions::OTHERS_EXEC ) ==    01,
                    "This method is not compatible due to changes in the permission enumeration." );

    target._<CHK>(""); // ensure valid target
    std::array<char, 9> result;
    std::array<char, 3> chars = {'r', 'w', 'x'};
    int bit          = 0400;
    size_t charIdx   = 0;
    while( bit )
    {
        result[charIdx]= perms & bit ? chars[charIdx % 3] : '-';
        charIdx++;
        bit >>= 1;
    }
    target << result;

    // This is the naive version that would not need the assertion above
    // target << (  (perms & FInfo::Permissions::OWNER_READ  ) == FInfo::Permissions::OWNER_READ    ? 'r'   : '-' )
    //        << (  (perms & FInfo::Permissions::OWNER_WRITE ) == FInfo::Permissions::OWNER_WRITE   ? 'w'   : '-' )
    //        << (  (perms & FInfo::Permissions::OWNER_EXEC  ) == FInfo::Permissions::OWNER_EXEC    ? 'x'   : '-' )
    //        << (  (perms & FInfo::Permissions::GROUP_READ  ) == FInfo::Permissions::GROUP_READ    ? 'r'   : '-' )
    //        << (  (perms & FInfo::Permissions::GROUP_WRITE ) == FInfo::Permissions::GROUP_WRITE   ? 'w'   : '-' )
    //        << (  (perms & FInfo::Permissions::GROUP_EXEC  ) == FInfo::Permissions::GROUP_EXEC    ? 'x'   : '-' )
    //        << (  (perms & FInfo::Permissions::OTHERS_READ ) == FInfo::Permissions::OTHERS_READ   ? 'r'   : '-' )
    //        << (  (perms & FInfo::Permissions::OTHERS_WRITE) == FInfo::Permissions::OTHERS_WRITE  ? 'w'   : '-' )
    //        << (  (perms & FInfo::Permissions::OTHERS_EXEC ) == FInfo::Permissions::OTHERS_EXEC   ? 'x'   : '-' )
    // ;
    return target;
}

AString& File::Format( Substring format, AString& target, lang::CurrentData targetData, NumberFormat* nf ) const
{
    if(nf == nullptr )
        nf= &NumberFormat::Computational;

    if ( targetData == lang::CurrentData::Clear )
        target.Reset();

    // this ensures that target is not nulled, as all other appends are NC-versions
    target._("");

    while ( format.IsNotEmpty() )
    {
        Box                toBeAdded;     // A box that most probably is set during the switch below. It will
                                          // be added potentially embedded in a TField.
        bool               isUpper=false; // if set during run, the result string will be converted to upper case
        AString            strBuffer;     // A string that might be filled and assigned to the result box (toBeAdded).
        system::Path pathBuffer;   // A path that might be filled and assigned to the result box (toBeAdded).

        // read n equal characters
        int   n=  1;
        character c=  format.ConsumeChar();
        while ( format.ConsumeChar(c) )
            ++n;

        if( isupper(c) )
        {
            c= character( tolower(c) );
            isUpper= true;
        }
        integer previousLength= target.Length();

        auto& value= Value();
        switch (c)
        {
            // path, name, stem, extension
            case 'n':
            {
                // read next character
                c= character(tolower(format.ConsumeChar()));
                switch(c)
                {
                    case 'a' :  toBeAdded= Name();                                 break;
                    case 's' :  toBeAdded= Stem();                                 break;
                    case 'e' :  toBeAdded= Extension();                            break;
                    case 'p' :
                    case 'f' :  AssemblePath( pathBuffer, lang::CurrentData::Keep );
                                if( c == 'f' )
                                {
                                    if( pathBuffer.CharAtEnd() != DIRECTORY_SEPARATOR )
                                        pathBuffer << DIRECTORY_SEPARATOR;
                                    pathBuffer << Name();
                                }
                                toBeAdded= pathBuffer;
                                break;

                    default:
                    {
                        ALIB_WARNING( "ALIB",  "Format Error: Token 'n' followed by unknown "
                                               "specifier '{}' in File::Format.", c )
                        target << "Format Error: Token 'n' followed by unknown specifier '" << c
                               << "' in File::Format.";
                        return target;
                    }
                }
            }
            break;


            case 'a':
                FormatAccessRights(strBuffer);
                toBeAdded= strBuffer;
                break;

            case 't': // Type
                     if( n == 1 )  toBeAdded= FInfo::TypeNames1Letter (value.Type());
                else if( n == 2 )  toBeAdded= FInfo::TypeNames2Letters(value.Type());
                else if( n == 3 )  toBeAdded= FInfo::TypeNames3Letters(value.Type());
                else               toBeAdded=                          value.Type();
                break;

            case 'l': // Symlink information
            {
                String4K symlinkInfo;
                if(    (    value.Type() == FInfo::Types::SYMBOLIC_LINK
                         || value.Type() == FInfo::Types::SYMBOLIC_LINK_DIR )
                    && value.Quality() >= FInfo::Qualities::RESOLVED )
                {
                    strBuffer <<  " -> " << value.GetLinkTarget();
                    if(     value.GetRealLinkTarget().IsNotEmpty()
                        && !value.GetLinkTarget().Equals( value.GetRealLinkTarget()) )
                        strBuffer <<  " (" << value.GetRealLinkTarget() <<  ")";
                    toBeAdded= strBuffer;
                }
                break;
            }

            case 'f': // IsCrossingFS() / IsArtificialFS()
                switch(c= character(tolower(format.ConsumeChar())))
                {
                    case 'x' : toBeAdded= (value.IsCrossingFS()   ? 'm' : '-') ; break;
                    case 'a' : toBeAdded= (value.IsArtificialFS() ? 'm' : '-') ; break;
                    default:
                    {
                        ALIB_WARNING( "ALIB", "Format Error: Unknown character {} after "
                                              "token 'f' in File::Format.", c )
                        target << "Format Error: Unknown character '" << c
                               << "' after token 'f' in File::Format.";
                        return target;
                    }
                }
                break;

            case 'h': // Quantity of hard links
                toBeAdded= value.QtyHardLinks();
                break;

            case 'q': // Quality
                if( n == 3 )  toBeAdded= FInfo::Qualities3Letters(value.Quality());
                else          toBeAdded=                          value.Quality();
                break;

            case 'd': // date
            {
                CalendarDateTime date;
                switch(c= character(tolower(format.ConsumeChar())))
                {
                    case 'm' : date= value.MDate(); break;
                    case 'b' : date= value.BDate(); break;
                    case 'c' : date= value.CDate(); break;
                    case 'a' : date= value.ADate(); break;
                    default:
                    {
                        ALIB_WARNING( "ALIB",
                                  "Format Error: Unknown character {} after token 'd' "
                                  "in File::Format.", c )
                        target << "Format Error: Unknown character '" << c
                               << "' after token 'd' in File::Format.";
                        return target;
                    }
                }

                String dateFormat= format.ConsumeField('{', '}' );
                if( dateFormat.IsEmpty() )
                    dateFormat= A_CHAR("dd. MMM yyyy HH:mm");
                date.Format( dateFormat, strBuffer );
                toBeAdded= strBuffer;
                break;
            }

            case 's': // size
            {
                bool automaticMode  = true;
                auto unit           = ByteSizeUnits::IEC;

                // entity specified in braces?
                if( format.CharAtStart() == '(' )
                {
                    format.ConsumeChar();
                    if( format.StartsWith<CHK,lang::Case::Ignore>(A_CHAR("SI")))
                    {
                        unit= ByteSizeUnits::SI;
                        format.ConsumeChars(2);
                    }
                    else if( format.StartsWith<CHK,lang::Case::Ignore>(A_CHAR("IEC")))
                    {
                        format.ConsumeChars(3);
                    }
                    else
                    {
                        enumrecords::Parse( format, unit );
                        automaticMode= false;
                    }

                    if( format.ConsumeChar() != ')' )
                    {
                        ALIB_WARNING( "ALIB",
                                  "Format Error: Expected closing brace ')' after unit specification with token 's'." )
                        target << "Format Error: Expected closing brace ')' after unit specification with token 's'.";
                        return target;
                    }
                }

                auto* ftreeNF= &GetFTree().GetNumberFormat();
                if( !automaticMode )
                {
                    // convert to given unit and output either a double or an integral.
                    ByteSizeIEC bs( value.Size() );
                    auto dval= bs.ConvertTo(unit);
                    if( unit==ByteSizeUnits::B || unit ==ByteSizeUnits::B_SI )
                        strBuffer << alib::Dec( uinteger(dval), 0, ftreeNF);
                    else
                        strBuffer << alib::Dec(          dval , 0, ftreeNF);
                }
                else
                {
                    // automatic output (automatically determine magnitude)
                    format::FormatByteSize( strBuffer, value.Size(), 900, 0, unit, *ftreeNF );
                }
                toBeAdded= strBuffer;
                break;
            }

            case 'o': // owner
            case 'g': // group
            {
                bool isOwner= c== 'o';
                c= format.ConsumeChar();

                if( c != 'i' && c != 'n' )
                {
                    ALIB_WARNING( "ALIB",
                              "Format Error: Expected 'i' or 'n' specifier after token 'o' and 'g'."
                              " Given: '{}'", n )
                    target << "Format Error: Expected 'i' or 'n' specifier after token 'o' and 'g'."
                              " Given: '" << c << "'";
                    return target;
                }
                bool isName= (c == 'n');

                if( isName )
                {
                    auto& resolver= GetFTree().GetOGResolver();
                    toBeAdded=  isOwner ? resolver.GetOwnerName(value)
                                        : resolver.GetGroupName(value);
                }
                else
                {
                    strBuffer << (isOwner ? value.Owner() : value.Group());
                    toBeAdded= strBuffer;
                }
                break;
            }

            // Extended directory info: sub-dirs, sub-files, access error, broken links
            case 'r':
            {
                // read next character
                c= character(tolower(format.ConsumeChar()));
                if(    !value.IsDirectory()
                    || value.Quality() < FInfo::Qualities::RECURSIVE   )
                {
                    toBeAdded= 0;
                    break;
                }

                FInfo::DirectorySums& dirInfo= value.Sums();
                switch(c)
                {
                    case 'd' :  toBeAdded= dirInfo.CountDirectories();       break;
                    case 'f' :  toBeAdded= dirInfo.CountNonDirectories();    break;
                    case 'e' :  toBeAdded= dirInfo.QtyErrsAccess;            break;
                    case 'b' :  toBeAdded= dirInfo.QtyErrsBrokenLink;        break;
                    default:
                    {
                        ALIB_WARNING( "ALIB",
                                  "Format Error: Token 'r' followed by unknown specifier '{}' "
                                  "in File::Format", c )
                        target << "Format Error: Token 'r' followed by unknown specifier '" << c
                               << "'in File::Format";
                        return target;
                    }
                }
            }
            break;


            //------------  single quotes and other characters ------------
            case '\'':
            {
                // one or more pairs of single quotes?
                if ( n > 1 )
                {
                    int pairs= n / 2;
                    target.InsertChars<NC>( '\'', pairs );
                    n-= (pairs * 2);
                }

                // one single quote?
                if ( n == 1 )
                {
                    // search end
                    integer end= format.IndexOf( '\'' );
                    if ( end < 1 )
                    {
                        ALIB_WARNING( "ALIB", "Format Error: Missing single Quote" )
                        target <<     "Format Error: Missing closing single quote character <'>" ;
                        return target;
                    }

                    target._<NC>( format, 0, end );
                    format.ConsumeChars<NC>( end + 1 );
                }
            }
            break;


            default: // otherwise: copy what was in
                target.InsertChars<NC>( c, n );
                break;
        } // switch(c)

        // field width, alignment specified in braces?
        int             width= -1;
        lang::Alignment alignment= lang::Alignment::Right;
        if( format.CharAtStart() == '{' )
        {
            format.ConsumeChar();
            format.ConsumeInt( width, &GetFTree().GetNumberFormat() );
            format.ConsumeChar(',');
            enumrecords::Parse( format, alignment );
            if( format.ConsumeChar() != '}' )
            {
                ALIB_WARNING( "ALIB",
                          "Format Error: Expected closing brace '}' with field specifier {width/alignment}." )
                target << "Format Error: Expected closing brace '}' with field specifier {width/alignment}.";
                return target;
            }
                target <<  Field( toBeAdded, width, alignment );
        }
        else
            target << toBeAdded;

        // upper case conversion
        if( isUpper )
            target.ToUpper(previousLength);
    }

    return target;
} // File::Format

void FFormat_File( const alib::Box& box, const alib::String& formatSpec, alib::NumberFormat& nf, alib::AString& target )
{
    box.Unbox<File>().Format( formatSpec.IsNotEmpty() ? formatSpec
                                                      : FILES.GetResource("FFMT"),
                              target,
                              lang::CurrentData::Keep,
                              &nf                                      );
}

} // namespace alib::files

//==================================================================================================
// struct AppendableTraits<File>
//==================================================================================================
#if !DOXYGEN

namespace alib::strings {

void AppendableTraits<files::File,nchar, lang::HeapAllocator>::operator()( TAString<nchar, lang::HeapAllocator>& target, const files::File& file )
{
    Path path;
    file.AssemblePath( path );
    target << path << file.GetFTree().Separator() << file.Name();
}

void AppendableTraits<files::File,wchar, lang::HeapAllocator>::operator()( TAString<wchar, lang::HeapAllocator>& target, const files::File& file )
{
    Path path;
    file.AssemblePath( path );
    target << path << file.GetFTree().Separator() << file.Name();
}

} // namespace [alib::strings]
#endif // DOXYGEN

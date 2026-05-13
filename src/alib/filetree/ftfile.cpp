using namespace alib::system;
namespace alib::filetree {

strings::TAString<PathCharType>&
FTFile::AssembleSymbolicPath( strings::TAString<PathCharType>&  target,
                              lang::Inclusion                   includeFilename )            const {

    static constexpr int STACK_SIZE= 256;
    FTree::CursorHandle   nStack[STACK_SIZE];

    FTFile   actNode= (*this);
    if(includeFilename == lang::Inclusion::Exclude)
        actNode= actNode.GetSymbolicParent();

    // if child and maxParent are the same, we do nothing
    if (actNode.IsRoot())
        return target;

    // build stack
    int               sp    =0;
    while( !actNode.IsRoot() ) {
        // local stack full? -> let a recursive call do the rest
        if(sp == STACK_SIZE) {
            FTFile(actNode).AssembleSymbolicPath( target, lang::Inclusion::Include );
            break;
        }

        if(actNode.HasSymbolicParent()) {
            auto parentNode= actNode;
            actNode= actNode.GetSymbolicParent();
            int i= 0;
            for(; i < sp; ++i) {
                if(nStack[i] == actNode.Export())
                    break;
            }
            if(i < sp) {
                auto parent= parentNode.Parent();
                if(parent.IsRoot())
                    break;
                nStack[sp++]= parent.Export();
                actNode= FTFile(parent.Parent());
        }   }
        else {
            nStack[sp++]= actNode.Export();
            actNode= actNode.Parent();
    }   }

    // unroll stack now from top to bottom
    while( --sp >= 0) {
        FTree::ConstCursorHandle ch    = nStack[sp];
        ConstCursor              actual= GetFTree().ImportCursor(ch);
        if( !actual.IsRoot() ) {
            if( target.CharAtEnd() != DIRECTORY_SEPARATOR  )
                target << DIRECTORY_SEPARATOR;
            target << actual.Name();
        }
        else
            target << DIRECTORY_SEPARATOR;
    }

    return target;
}

AString&  FTFile::FormatAccessRights(AString& target)                                        const {
    int perms= int(Value().Perms());
    // This is a "hard-coded" optimized approach. It only works unless the values of enum
    // Permissions are as they have been since decades...
    static_assert(  int(FileStatus::Permissions::OWNER_READ  ) ==  0400 &&
                    int(FileStatus::Permissions::OWNER_WRITE ) ==  0200 &&
                    int(FileStatus::Permissions::OWNER_EXEC  ) ==  0100 &&
                    int(FileStatus::Permissions::GROUP_READ  ) ==   040 &&
                    int(FileStatus::Permissions::GROUP_WRITE ) ==   020 &&
                    int(FileStatus::Permissions::GROUP_EXEC  ) ==   010 &&
                    int(FileStatus::Permissions::OTHERS_READ ) ==    04 &&
                    int(FileStatus::Permissions::OTHERS_WRITE) ==    02 &&
                    int(FileStatus::Permissions::OTHERS_EXEC ) ==    01,
                    "This method is not compatible due to changes in the permission enumeration." );

    target._<CHK>(""); // ensure valid target
    std::array<char, 9> result;
    std::array<char, 3> chars = {'r', 'w', 'x'};
    int bit          = 0400;
    size_t charIdx   = 0;
    while( bit ) {
        result[charIdx]= perms & bit ? chars[charIdx % 3] : '-';
        charIdx++;
        bit >>= 1;
    }
    target << result;

    // This is the naive version that would not need the assertion above
    // target << (  (perms & FileStatus::Permissions::OWNER_READ  ) == FileStatus::Permissions::OWNER_READ    ? 'r'   : '-' )
    //        << (  (perms & FileStatus::Permissions::OWNER_WRITE ) == FileStatus::Permissions::OWNER_WRITE   ? 'w'   : '-' )
    //        << (  (perms & FileStatus::Permissions::OWNER_EXEC  ) == FileStatus::Permissions::OWNER_EXEC    ? 'x'   : '-' )
    //        << (  (perms & FileStatus::Permissions::GROUP_READ  ) == FileStatus::Permissions::GROUP_READ    ? 'r'   : '-' )
    //        << (  (perms & FileStatus::Permissions::GROUP_WRITE ) == FileStatus::Permissions::GROUP_WRITE   ? 'w'   : '-' )
    //        << (  (perms & FileStatus::Permissions::GROUP_EXEC  ) == FileStatus::Permissions::GROUP_EXEC    ? 'x'   : '-' )
    //        << (  (perms & FileStatus::Permissions::OTHERS_READ ) == FileStatus::Permissions::OTHERS_READ   ? 'r'   : '-' )
    //        << (  (perms & FileStatus::Permissions::OTHERS_WRITE) == FileStatus::Permissions::OTHERS_WRITE  ? 'w'   : '-' )
    //        << (  (perms & FileStatus::Permissions::OTHERS_EXEC ) == FileStatus::Permissions::OTHERS_EXEC   ? 'x'   : '-' )
    // ;
    return target;
}

AString& FTFile::Format( Substring format, AString& target, lang::CurrentData targetData, NumberFormat* nf ) const {
    if(nf == nullptr )
        nf= &NumberFormat::Computational;

    if ( targetData == lang::CurrentData::Clear )
        target.Reset();

    // this ensures that target is not nulled, as all other appends are NC-versions
    target._("");

    while ( format.IsNotEmpty() ) {
        Box           toBeAdded;     // A box that most probably is set during the switch below. It will
                                     // be added potentially embedded in a TField.
        bool          isUpper=false; // if set during run, the result string will be converted to upper case
        Path          pathBuffer;    // A path that might be filled and assigned to the result box (toBeAdded).
        String256     strBuffer;     // A string that might be filled and assigned to the result box (toBeAdded).
        strBuffer .DbgDisableBufferReplacementWarning();
        pathBuffer.DbgDisableBufferReplacementWarning();

        // read n equal characters
        int   n=  1;
        character c=  format.ConsumeChar();
        while ( format.ConsumeChar(c) )
            ++n;

        if( isupper(c) ) {
            c= character( tolower(c) );
            isUpper= true;
        }
        integer previousLength= target.Length();

        auto& value= Value();
        switch (c) {
            // path, name, stem, extension
            case 'n':
            {
                // read next character
                c= character(tolower(format.ConsumeChar()));
                switch(c) {
                    case 'a' :  toBeAdded= Name();                                 break;
                    case 's' :  toBeAdded= Stem();                                 break;
                    case 'e' :  toBeAdded= Extension();                            break;
                    case 'p' :
                    case 'f' :  AssembleSymbolicPath( pathBuffer, c == 'f'
                                                                  ? lang::Inclusion::Include
                                                                  : lang::Inclusion::Exclude  );
                                toBeAdded= pathBuffer;
                                break;
                    case 'r' :
                    case 'x' :  AssembleRealPath    ( pathBuffer, c == 'x'
                                                                  ? lang::Inclusion::Include
                                                                  : lang::Inclusion::Exclude  );
                                toBeAdded= pathBuffer;
                                break;

                    default:
                    {
                        ALIB_WARNING( "ALIB",  "Format Error: Token 'n' followed by unknown "
                                               "specifier '{}' in FTFile::Format.", c )
                        target << "Format Error: Token 'n' followed by unknown specifier '" << c
                               << "' in FTFile::Format.";
                        return target;
            }   }   }
            break;


            case 'a':
                FormatAccessRights(strBuffer);
                toBeAdded= strBuffer;
                break;

            case 't': // Type
                     if( n == 1 )  toBeAdded= FTValue::TypeNames1Letter (value.Type());
                else if( n == 2 )  toBeAdded= FTValue::TypeNames2Letters(value.Type());
                else if( n == 3 )  toBeAdded= FTValue::TypeNames3Letters(value.Type());
                else               toBeAdded=                          value.Type();
                break;

            case 'l': // Symlink information
            {
                String4K symlinkInfo;
                if(    (    value.Type() == FileStatus::Types::SYMBOLIC_LINK
                         || value.Type() == FileStatus::Types::SYMBOLIC_LINK_DIR )
                    && value.ScanState() >= FTValue::ScanStates::RESOLVED )
                {
                    strBuffer <<  " -> " << value.GetLinkTarget();
                    if(     value.GetRealLinkTarget().IsNotEmpty()
                        && !value.GetLinkTarget().Equals( value.GetRealLinkTarget()) )
                        strBuffer <<  " (" << value.GetRealLinkTarget() <<  ")";
                    toBeAdded= strBuffer;
                }
                break;
            }
            case 'b': // Symbolic parent
            {
                if(value.symParent != 0) {
                    const FTFile symbolicParent= GetSymbolicParent();
                    pathBuffer <<  " <- ";
                    symbolicParent.AssemblePath(pathBuffer, lang::CurrentData::Keep);
                    toBeAdded= pathBuffer;
                }
                break;
            }

            case 'f': // IsCrossingFS() / IsArtificialFS()
                switch(c= character(tolower(format.ConsumeChar()))) {
                    case 'x' : toBeAdded= (value.IsCrossingFS()   ? 'm' : '-') ; break;
                    case 'a' : toBeAdded= (value.IsArtificialFS() ? 'm' : '-') ; break;
                    default:
                    {
                        ALIB_WARNING( "ALIB", "Format Error: Unknown character {} after "
                                              "token 'f' in FTFile::Format.", c )
                        target << "Format Error: Unknown character '" << c
                               << "' after token 'f' in FTFile::Format.";
                        return target;
                }   }
                break;

            case 'h': // Quantity of hard links
                if( value.QtyHardLinks() != uint32_t(-1) )   toBeAdded= value.QtyHardLinks();
                else                                         toBeAdded= "-1";
                break;

            case 'q': // ScanState
                if( n == 3 )  toBeAdded= FTValue::ScanStates3Letters(value.ScanState());
                else          toBeAdded=                           value.ScanState();
                break;

            case 'd': // date
            {
                CalendarDateTime date;
                switch(c= character(tolower(format.ConsumeChar()))) {
                    case 'm' : date= value.MDate(); break;
                    case 'b' : date= value.BDate(); break;
                    case 'c' : date= value.CDate(); break;
                    case 'a' : date= value.ADate(); break;
                    default:
                    {
                        ALIB_WARNING( "ALIB",
                                  "Format Error: Unknown character {} after token 'd' "
                                  "in FTFile::Format.", c )
                        target << "Format Error: Unknown character '" << c
                               << "' after token 'd' in FTFile::Format.";
                        return target;
                }   }

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
                if( format.CharAtStart() == '(' ) {
                    format.ConsumeChar();
                    if( format.StartsWith<CHK,lang::Case::Ignore>(A_CHAR("SI"))) {
                        unit= ByteSizeUnits::SI;
                        format.ConsumeChars(2);
                    }
                    else if( format.StartsWith<CHK,lang::Case::Ignore>(A_CHAR("IEC"))) {
                        format.ConsumeChars(3);
                    } else {
                        enumrecords::Parse( format, unit );
                        automaticMode= false;
                    }

                    if( format.ConsumeChar() != ')' ) {
                        ALIB_WARNING( "ALIB",
                                  "Format Error: Expected closing brace ')' after unit specification with token 's'." )
                        target << "Format Error: Expected closing brace ')' after unit specification with token 's'.";
                        return target;
                }   }

                auto* ftreeNF= &GetFTree().GetNumberFormat();
                if( !automaticMode ) {
                    // convert to given unit and output either a double or an integral.
                    ByteSizeIEC bs( value.Size() );
                    auto dval= bs.ConvertTo(unit);
                    if( unit==ByteSizeUnits::B || unit ==ByteSizeUnits::B_SI )
                        strBuffer << alib::Dec( uinteger(dval), 0, ftreeNF);
                    else
                        strBuffer << alib::Dec(          dval , 0, ftreeNF);
                } else {
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

                if( c != 'i' && c != 'n' ) {
                    ALIB_WARNING( "ALIB",
                              "Format Error: Expected 'i' or 'n' specifier after token 'o' and 'g'."
                              " Given: '{}'", n )
                    target << "Format Error: Expected 'i' or 'n' specifier after token 'o' and 'g'."
                              " Given: '" << c << "'";
                    return target;
                }
                bool isName= (c == 'n');

                if( isName ) {
                    auto& resolver= GetFTree().GetOGResolver();
                    toBeAdded=  isOwner ? resolver.GetOwnerName(value)
                                        : resolver.GetGroupName(value);
                } else {
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
                    || value.ScanState() < FTValue::ScanStates::RECURSIVE   )
                {
                    toBeAdded= 0;
                    break;
                }

                FTValue::DirectorySums& dirInfo= value.Sums();
                switch(c) {
                    case 'd' :  toBeAdded= dirInfo.CountDirectories();       break;
                    case 'f' :  toBeAdded= dirInfo.CountNonDirectories();    break;
                    case 'e' :  toBeAdded= dirInfo.QtyErrsAccess;            break;
                    case 'b' :  toBeAdded= dirInfo.QtyErrsBrokenLink;        break;
                    default:
                    {
                        ALIB_WARNING( "ALIB",
                                  "Format Error: Token 'r' followed by unknown specifier '{}' "
                                  "in FTFile::Format", c )
                        target << "Format Error: Token 'r' followed by unknown specifier '" << c
                               << "'in FTFile::Format";
                        return target;
            }   }   }
            break;


          //--------------------------- single quotes and other characters -------------------------
            case '\'':
            {
                // one or more pairs of single quotes?
                if ( n > 1 ) {
                    int pairs= n / 2;
                    target.InsertChars<NC>( '\'', pairs );
                    n-= (pairs * 2);
                }

                // one single quote?
                if ( n == 1 ) {
                    // search end
                    integer end= format.IndexOf( '\'' );
                    if ( end < 1 ) {
                        ALIB_WARNING( "ALIB", "Format Error: Missing single Quote" )
                        target <<     "Format Error: Missing closing single quote character <'>" ;
                        return target;
                    }

                    target._<NC>( format, 0, end );
                    format.ConsumeChars<NC>( end + 1 );
            }   }
            break;


            default: // otherwise: copy what was in
                target.InsertChars<NC>( c, n );
                break;
        } // switch(c)

        // field width, alignment specified in braces?
        int             width= -1;
        lang::Alignment alignment= lang::Alignment::Right;
        if( format.CharAtStart() == '{' ) {
            format.ConsumeChar();
            format.ConsumeInt( width, &GetFTree().GetNumberFormat() );
            format.ConsumeChar(',');
            enumrecords::Parse( format, alignment );
            if( format.ConsumeChar() != '}' ) {
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
} // FTFile::Format

void FFormat_File( const Box& box, const String& formatSpec, NumberFormat& nf, AString& target ) {
    box.Unbox<FTFile>().Format( formatSpec.IsNotEmpty() ? formatSpec
                                                        : FILETREE.GetResource("FFMT"),
                                target,
                                lang::CurrentData::Keep,
                                &nf                                      );
}

} // namespace alib::filetree

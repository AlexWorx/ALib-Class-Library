#if ALIB_CAMP_RESOURCE_COMPILATION

namespace alib::camp {

#include "ALib.Lang.CIFunctions.H"

void  LoadResourceFile(Path& rcFilePath, ResourceList& destination, RCErrorList& errors){
  //-------------------------------------- open file input file ------------------------------------
    MappedFile       rcFile;
    std::errc errc= rcFile.Open(rcFilePath.Terminate());
    if(errc != std::errc()) {
        errors.emplace_back( ResourceFileErrors::RCFileNotFoundOrAccessible,
                             0,0,0,
                             PathString(errors.get_allocator().GetAllocator(), rcFilePath));
        return;
    }
    MappedFile::Data<char> mfc= rcFile.GetData<char>();

    MonoAllocator ma(ALIB_DBG("CampRCTemp",) 16);
    HashSet<MonoAllocator, NString> dedupSet(ma);

  //-------------------------------------------- helpers -------------------------------------------
    integer     lineStartRemaining= mfc.Remaining();
    int         lineNo= 1;
    NString512  rcName;
    String4K    rcVal;

    auto isWS= [](char c) -> bool {
        return c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v';
    };

    auto isCommentStart= [&](char c) -> bool  {
        return c == '#'
            || (c == '/' && mfc.Remaining() > 0 && *mfc == '/');
    };

    auto consumeEOL= [&]() {
        while(!mfc.IsEOF()) {
            char c= mfc.Next<NC>();
            if(c == '\n') {
                lineStartRemaining= mfc.Remaining();
                ++lineNo;
                return;
        }   }
    };

    auto hexValue= [](char c) -> int {
        if(c >= '0' && c <= '9') return c - '0';
        if(c >= 'a' && c <= 'f') return 10 + (c - 'a');
        if(c >= 'A' && c <= 'F') return 10 + (c - 'A');
        return -1;
    };

    auto appendUtf8= [&](uint32_t codePoint) {
        // Replace invalid code points (including UTF-16 surrogates) with U+FFFD.
        if(    codePoint > 0x10FFFFu
            || (codePoint >= 0xD800u && codePoint <= 0xDFFFu) )
            codePoint= 0xFFFDu;

        if(codePoint <= 0x7Fu) {
            rcVal << char(codePoint);
            return;
        }
        if(codePoint <= 0x7FFu) {
            rcVal << char(0xC0u | ((codePoint >> 6) & 0x1Fu));
            rcVal << char(0x80u | ( codePoint       & 0x3Fu));
            return;
        }
        if(codePoint <= 0xFFFFu) {
            rcVal << char(0xE0u | ((codePoint >> 12) & 0x0Fu));
            rcVal << char(0x80u | ((codePoint >>  6) & 0x3Fu));
            rcVal << char(0x80u | ( codePoint        & 0x3Fu));
            return;
        }
        rcVal << char(0xF0u | ((codePoint >> 18) & 0x07u));
        rcVal << char(0x80u | ((codePoint >> 12) & 0x3Fu));
        rcVal << char(0x80u | ((codePoint >>  6) & 0x3Fu));
        rcVal << char(0x80u | ( codePoint        & 0x3Fu));
    };

    auto appendUnicodeEscapeFromLineBuf= [&](const NString4K& lineBuf, integer& idx) -> bool {
        if(idx + 4 >= lineBuf.Length())
            return false;

        uint32_t codePoint= 0;
        for(integer d= 1; d <= 4; ++d) {
            int hv= hexValue(lineBuf.CharAt(idx + d));
            if(hv < 0)
                return false;
            codePoint= (codePoint << 4) | uint32_t(hv);
        }

        appendUtf8(codePoint);
        idx += 4;
        return true;
    };

    auto appendUnicodeEscapeFromMappedFile= [&]() -> bool {
        auto view= mfc;
        uint32_t codePoint= 0;
        for(int d= 0; d < 4; ++d) {
            if(view.IsEOF())
                return false;
            int hv= hexValue(view.Next<NC>());
            if(hv < 0)
                return false;
            codePoint= (codePoint << 4) | uint32_t(hv);
        }

        appendUtf8(codePoint);
        mfc.Skip(4);
        return true;
    };

    auto appendEscape= [&](char esc, bool& hadBackslashEscape) {
        // Processes one escape sequence character 'esc' (the char after '\').
        switch( esc ) {
            case ' ':  rcVal << ' ';  break;
            case 'n':  rcVal << '\n'; break;
            case 'r':  rcVal << '\r'; break;
            case 't':  rcVal << '\t'; break;
            case 'a':  rcVal << '\a'; break;
            case 'b':  rcVal << '\b'; break;
            case 'f':  rcVal << '\f'; break;
            case 'v':  rcVal << '\v'; break;
            case '\\': rcVal << '\\'; break;
            case '"':  rcVal << '"' ; break;
            default:   rcVal << esc  ; break;
        }
        hadBackslashEscape= false;
    };

    auto readIndentCount= [&](MappedFile::Data<char>& view) -> int {
        // counts leading spaces/tabs from current position up to first non-(space/tab) or newline/EOF
        int cnt= 0;
        while(!view.IsEOF()) {
            char c= *view;
            if(c == ' ' || c == '\t') { ++cnt; view.Next<NC>(); continue; }
            break;
        }
        return cnt;
    };

    // Read a logical "block" content, using indentation rules described in the dox above.
    // Parameters:
    //  - keyIndent: indentation (spaces/tabs count) of the key line
    //  - folded:    true for '>' folded, false for '|' literal
    //  - compact:   true to feed text into compact lexer (whitespace ignored outside quotes)
    auto readBlockContent= [&](int keyIndent, bool folded, bool compact) {
        rcVal.Reset();

        // move to next line (consume remainder of key line up to '\n')
        while(!mfc.IsEOF()) {
            char c= mfc.Next<NC>();
            if(c == '\n') {
                lineStartRemaining= mfc.Remaining();
                ++lineNo;
                break;
        }   }
        if(mfc.IsEOF())
            return;

        int    stripIndent       =     0;
        bool   stripIndentKnown  = false;
        bool   prevWasEmpty      =  true; // for folded blocks and compact blocks (space insertion)
        bool   inQuotes          = false;
        bool   hadBackslashEscape= false;

        while(!mfc.IsEOF()) {
            // snapshot view to measure indentation without losing data if block ends
            auto view= mfc;
            int indent= readIndentCount(view);

            // check end-of-block: first non-empty line with indentation <= keyIndent
            char first= view.IsEOF() ? '\0' : *view;
            bool isEmptyLine= (first == '\n') || (first == '\r' && view.Remaining() > 0 && *view == '\n');
            if(!isEmptyLine && indent <= keyIndent)
                break;

            // consume indentation from real stream (same as measured)
            mfc.Skip(indent);

            // normalize CRLF: if we see '\r' before '\n', ignore '\r'
            // now read the line into a temporary buffer (up to '\n' or EOF)
            NString4K lineBuf;
            while(!mfc.IsEOF()) {
                char c= mfc.Next<NC>();
                if(c == '\r')     continue;
                if(c == '\n')     break;
                lineBuf << c;
            }

            // line finished -> update line counters (we consumed '\n' unless EOF)
            if(!mfc.IsEOF() || (mfc.IsEOF() && (lineBuf.IsNotEmpty()))) {
                lineStartRemaining= mfc.Remaining();
                ++lineNo;
            }

            // empty line handling
            if(lineBuf.IsEmpty()) {
                if(compact) {
                    // In compact blocks, newline is just whitespace unless inside quotes.
                    if(inQuotes)
                        rcVal << '\n';
                }
                else if(folded) {
                    // paragraph break
                    if(!prevWasEmpty)
                        rcVal << '\n';
                    rcVal << '\n';
                }
                else {
                    // literal: keep empty line
                    rcVal << '\n';
                }

                prevWasEmpty= true;
                continue;
            }

            // Determine indent-to-strip based on the first non-empty line.
            // We've already consumed 'indent' spaces from the file, so lineBuf has no leading spaces.
            // But we want to preserve relative indentation: lines more indented than the first should keep their extra indent.
            if(!stripIndentKnown) {
                stripIndent     = indent;
                stripIndentKnown= true;
            }

            // Calculate relative indentation: how many extra spaces does this line have?
            int relativeIndent= indent - stripIndent;

            // feed line content
            if(compact) {
                // Compact lexer: ignore whitespace outside quotes; allow quoted islands; process escapes.
                // In a block, we treat end-of-line as whitespace (ignored outside quotes).
                for(integer i= 0; i < lineBuf.Length(); ++i) {
                    char c= lineBuf.CharAt(i);
                    if(hadBackslashEscape) {
                        if(c == 'u' && appendUnicodeEscapeFromLineBuf(lineBuf, i)) {
                            hadBackslashEscape= false;
                            continue;
                        }
                        appendEscape(c, hadBackslashEscape);
                        continue;
                    }
                    if(c == '\\')          { hadBackslashEscape= true; continue; }
                    if(c == '"')           { inQuotes= !inQuotes; continue; }

                    if(!inQuotes && (c == ' ' || c == '\t')) // ignore outside quotes
                        continue;

                    rcVal << c;
                }
                // line end => whitespace outside quotes, newline inside quotes
                if(inQuotes)
                    rcVal << '\n';
                prevWasEmpty= false;
                continue;
            }

            // Non-compact blocks: process escapes and apply literal/folded joining.
            if(folded) {
                if(!prevWasEmpty && !(rcVal.IsNotEmpty() && rcVal.CharAtEnd() == '\n'))
                    rcVal << ' ';
            }

            // Add relative indentation
            for(int i= 0; i < relativeIndent; ++i)
                rcVal << ' ';
            
            for(integer i= 0; i < lineBuf.Length(); ++i) {
                char c= lineBuf.CharAt(i);
                if(hadBackslashEscape) {
                    if(c == 'u' && appendUnicodeEscapeFromLineBuf(lineBuf, i)) {
                        hadBackslashEscape= false;
                        continue;
                    }
                    appendEscape(c, hadBackslashEscape);
                    continue;
                }
                if(c == '\\')          { hadBackslashEscape= true; continue; }
                rcVal << c;
            }
            if(!folded) // literal keeps newline
                rcVal << '\n';

            prevWasEmpty= false;
        }

        if(inQuotes) {
            errors.emplace_back( ResourceFileErrors::ErrLineEndWhileInQuotes,
                                 lineNo, 0, 0,
                                 PathString(errors.get_allocator().GetAllocator(), rcFilePath));
        }

        // If the last char was a dangling backslash escape marker, treat it as a literal '\'
        // (Same behavior as "unknown escape" case already yields literal; here it's just missing the next char.)
        if(hadBackslashEscape)
            rcVal << '\\';
        
        // Strip trailing newlines from any block scalar. If users want trailing newlines,
        // they can add them explicitly using escape sequences (e.g. "\n") at the end of the last line.
        while(rcVal.IsNotEmpty() && rcVal.CharAtEnd() == '\n')
            rcVal.ShortenBy(1);
    };

    auto nextResourcePair= [&]() -> bool
    {
        while( !mfc.IsEOF() ) {
            // --- determine indentation of this (potential) key line ---
            int keyIndent= 0;
            {
                auto view= mfc;
                keyIndent= readIndentCount(view);
            }

            char c= mfc.Next<NC>();

            // Skip leading whitespace and empty lines.
            while( c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v' || c == '\n' ) {
                if( c == '\n' ) {
                    lineStartRemaining= mfc.Remaining();
                    ++lineNo;

                    // update keyIndent for new line
                    auto view= mfc;
                    keyIndent= readIndentCount(view);
                }
                if( mfc.IsEOF() )
                    return false;
                c= mfc.Next<NC>();
            }

            // skip comment line
            if( isCommentStart(c) ) {
                consumeEOL();
                continue;
            }

            rcName.Reset();
            rcVal .Reset();

          //---------------------------- Read and validate resource name ---------------------------
            while( c != 0 ) {
                unsigned char uc= static_cast<unsigned char>(c);
                if( uc == '\n' ) {
                    if(mfc.IsEOF())
                        break;
                    c= mfc.Next<NC>();
                    continue;
                }
                if( uc < 33 || uc > 127 || uc == 34 )
                { // printable characters, excluding '"'
                    errors.emplace_back( ResourceFileErrors::ErrIllegalResourceName,
                                         lineNo, 0, 0, uc);
                    if(mfc.IsEOF())
                        break;
                    c= mfc.Next<NC>();
                    continue;
                }
                if( c == '=' )
                    break;

                if( rcName.Length() >= 512 - 1 ) {
                    errors.emplace_back( ResourceFileErrors::ErrNameTooLong,
                                         lineNo, lineStartRemaining - mfc.Remaining(), 0,
                                         NString(errors.get_allocator().GetAllocator(), rcName));
                    if(mfc.IsEOF())
                        break;
                    c= mfc.Next<NC>();
                    continue;
                }

                rcName << c;

                if( mfc.IsEOF() )
                    break;

                c= mfc.Next<NC>();
                if( c == '=' || isWS(c) || c == '\n' )
                    break;
            }

            // empty line?
            if( rcName.IsEmpty() ) {
                consumeEOL();
                continue;
            }

            // Skip separator whitespace between name and value.
            while( isWS(c) ) {
                if( mfc.IsEOF() )
                    break;
                c= mfc.Next<NC>();
            }

            // Optional '=' separator
            if( c == '=' ) {
                if(!mfc.IsEOF())
                    c= mfc.Next<NC>();

                while( isWS(c) ) {
                    if( mfc.IsEOF() )
                        break;
                    c= mfc.Next<NC>();
            }   }

            // value may be empty (end of line / EOF)
            if( mfc.IsEOF() )
                return true;

          //------------------------------------- Value parsing ------------------------------------
          // Note: c is the first non-whitespace character after optional '=' (or it may be '\n').
            if( c == '\n' ) {
                // empty value
                lineStartRemaining= mfc.Remaining();
                ++lineNo;
                return true;
            }

            // 3) Block scalars
            if( c == '|' || c == '>' ) {
                bool folded= (c == '>');
                readBlockContent(keyIndent, folded, /*compact*/ false);
                return true;
            }

            // 4) Compact scalars
            if( c == '~' ) {
                if(mfc.IsEOF())
                    return true;

                // check for block compact "~|"
                char next= mfc.IsEOF() ? '\0' : mfc.Next<NC>();
                if(next == '|' ) {
                    readBlockContent(keyIndent, /*folded*/ false, /*compact*/ true);
                    return true;
                }

                // inline compact: parse until end-of-line
                bool inQuotes= false;
                bool hadBackslashEscape= false;

                // We consumed one char after '~' into 'next' already; start with that.
                c= next;

                while( true ) {
                    if( c == '\n' ) {
                        if( inQuotes )
                            errors.emplace_back( ResourceFileErrors::ErrLineEndWhileInQuotes,
                              lineNo, 0, 0,
                              PathString(errors.get_allocator().GetAllocator(), rcFilePath));

                        lineStartRemaining= mfc.Remaining();
                        ++lineNo;
                        return true;
                    }

                    if( hadBackslashEscape ) {
                        if(c == 'u' && appendUnicodeEscapeFromMappedFile()) {
                            hadBackslashEscape= false;
                        } else {
                            appendEscape(c, hadBackslashEscape);
                    }   }
                    else if( c == '\\' )      { hadBackslashEscape= true; }
                    else if( c == '"' )       { inQuotes= !inQuotes; }
                    else if( !inQuotes && (c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v') ) {
                        // ignore
                    } else {
                        rcVal << c;
                    }

                    if( mfc.IsEOF() ) {
                        if(hadBackslashEscape)
                            rcVal << '\\';
                        if(inQuotes)
                            errors.emplace_back( ResourceFileErrors::ErrLineEndWhileInQuotes,
                                lineNo,
                                0,0,
                             PathString(errors.get_allocator().GetAllocator(), rcFilePath));
                        return true;
                    }

                    c= mfc.Next<NC>();
            }   }

            // 1) Quoted scalar
            if( c == '"' ) {
                bool hadBackslashEscape= false;
                while(true) {
                    if(mfc.IsEOF()) {
                        if(hadBackslashEscape)
                            rcVal << '\\';
                        errors.emplace_back(
                                    ResourceFileErrors::ErrLineEndWhileInQuotes, lineNo, 0, 0,
                                    PathString(errors.get_allocator().GetAllocator(), rcFilePath) );
                        return true;
                    }

                    c= mfc.Next<NC>();

                    if( c == '\n' ) {
                        errors.emplace_back(
                                     ResourceFileErrors::ErrLineEndWhileInQuotes, lineNo, 0, 0,
                                     PathString(errors.get_allocator().GetAllocator(), rcFilePath));
                        lineStartRemaining= mfc.Remaining();
                        ++lineNo;
                        return true;
                    }

                    if( hadBackslashEscape ) {
                        if(c == 'u' && appendUnicodeEscapeFromMappedFile()) {
                            hadBackslashEscape= false;
                            continue;
                        }
                        appendEscape(c, hadBackslashEscape);
                        continue;
                    }
                    if( c == '\\' )          { hadBackslashEscape= true; continue; }
                    if( c == '"' )           break; // end quote

                    rcVal << c;
                }

                // ignore trailing whitespace, then consume end of line (or EOF)
                while(!mfc.IsEOF()) {
                    char t= *mfc;
                    if(t == '\r') { mfc.Next<NC>(); continue; }
                    if(isWS(t))   { mfc.Next<NC>(); continue; }
                    if(t == '\n') { mfc.Next<NC>(); lineStartRemaining= mfc.Remaining(); ++lineNo; break; }
                    break; // any other character: treat as part of nothing (strict), but we just stop here
                }

                return true;
            }

            // 2) Plain scalar: rest-of-line (trim trailing whitespace), process escapes
            {
                bool hadBackslashEscape= false;
                NString4K lineBuf;
                while(true) {
                    if( c == '\r' ) {
                        if(mfc.IsEOF())
                            break;
                        c= mfc.Next<NC>();
                        continue;
                    }

                    if( c == '\n' )
                        break;

                    lineBuf << c;

                    if( mfc.IsEOF() )
                        break;

                    c= mfc.Next<NC>();
                }

                // trim trailing whitespace of the plain scalar (only spaces/tabs/etc., not newline)
                while(lineBuf.IsNotEmpty()) {
                    char last= lineBuf.CharAtEnd();
                    if(last == ' ' || last == '\t' || last == '\r' || last == '\f' || last == '\v')
                        lineBuf.ShortenBy(1);
                    else
                        break;
                }

                // process escapes
                for(integer i= 0; i < lineBuf.Length(); ++i) {
                    char ch= lineBuf.CharAt(i);
                    if(hadBackslashEscape) {
                        if(ch == 'u' && appendUnicodeEscapeFromLineBuf(lineBuf, i)) {
                            hadBackslashEscape= false;
                            continue;
                        }
                        appendEscape(ch, hadBackslashEscape);
                        continue;
                    }
                    if(ch == '\\')         { hadBackslashEscape= true; continue; }
                    rcVal << ch;
                }
                if(hadBackslashEscape)
                    rcVal << '\\';

                if(c == '\n') {
                    lineStartRemaining= mfc.Remaining();
                    ++lineNo;
                }

                return true;
        }   }

        return false;
    }; // lambda function nextResourcePair

  //------------------------------------------- main loop ------------------------------------------
    while( nextResourcePair() ) {
        // allocate name and value in the global allocator
        NString name=  NString(monomem::GLOBAL_ALLOCATOR, rcName);
         String value=  String(monomem::GLOBAL_ALLOCATOR, rcVal );

        // add to resources and to ordered list
        destination.emplace_back(ResourceListEntry{name, value, lineNo-1});

        if( !dedupSet.InsertIfNotExistent(name).second ) {
            // search first occurrence
            int firstOccurrence= -1;
            for( const auto& entry : destination )
                if( entry.Key == name ) { firstOccurrence= entry.LineNo; break; }
            errors.emplace_back( ResourceFileErrors::DuplicateResourceName,
                                 lineNo-1, 0, firstOccurrence, name );
}   }   }


void PatchCPPFile( const Path& cppFilePath, const ResourceList& resources, RCErrorList& errors,
                   bool dryrun ) {

    MonoAllocator& ma= resources.get_allocator().GetAllocator();
    MonoAllocator::Resetter resetter(ma);
    TextFile<1024> file(ma);
    std::errc errc= file.Read(cppFilePath);
    if(errc != std::errc()) {
        errors.emplace_back( ResourceFileErrors::CPPFileNotFoundOrAccessible,
                             0,0,0,
                             PathString(errors.get_allocator().GetAllocator(), cppFilePath));
        return;
    }

    // search start/end of replacement lines
    integer startLine= 0;
    for( ; startLine < file.Size(); ++startLine ) {
        if( file.At(startLine).IndexOf("ALIB-RESOURCE-COMPILER-REPLACEMENT-START") > 0)
            break;
    }
    ++startLine;
    integer endLine= startLine;
    for( ; endLine < file.Size(); ++endLine ) {
        if( file.At(endLine).IndexOf("ALIB-RESOURCE-COMPILER-REPLACEMENT-END") > 0)
            break;
    }
    if( startLine >= file.Size() ) {
        errors.emplace_back( ResourceFileErrors::CPPFileMissingStartMarker,
                             0,0,0,
                             PathString(errors.get_allocator().GetAllocator(), cppFilePath));
        return;
    }
    if( endLine >= file.Size() ) {
        errors.emplace_back( ResourceFileErrors::CPPFileMissingEndMarker,
                             0,0,0,
                             PathString(errors.get_allocator().GetAllocator(), cppFilePath));
        return;
    }

    // adjust file length
    integer lineDiff= integer(resources.size()) -  (endLine - startLine);
         if( lineDiff < 0 ) file.erase(file.begin() + endLine + lineDiff, file.begin() + endLine);
    else if( lineDiff > 0 ) file.insert(file.begin()+ endLine, size_t(lineDiff),
                            TextFile<1024>::StoredType(file.GetAllocator(), "") );


    // loop over resources and replace
    integer maxNameLen= 0;
    for( const auto& entry : resources )
        maxNameLen= (std::max)( maxNameLen, entry.Key.Length() );
    maxNameLen= (std::min)( maxNameLen + 4 + 2, integer(30) );

    auto hexDigit= [](uint32_t value) -> char {
        value&= 0xFu;
        return value < 10u ? char('0' + value) : char('A' + (value - 10u));
    };

    auto appendHexEscape= [&](NString1K& target, uint32_t codePoint) {
        if(codePoint <= 0xFFFFu) {
            target._<NC>("\\u");
            for(int shift= 12; shift >= 0; shift-= 4)
                target << hexDigit(codePoint >> uint32_t(shift));
            return;
        }
        target._<NC>("\\U");
        for(int shift= 28; shift >= 0; shift-= 4)
            target << hexDigit(codePoint >> uint32_t(shift));
    };

    auto decodeUtf8CodePoint= [&](const String& src, integer& idx) -> uint32_t {
        auto readByteAt= [&](integer pos) -> unsigned char {
            return static_cast<unsigned char>(src.CharAt(pos));
        };

        unsigned char b0= readByteAt(idx);
        if(b0 < 0x80u)
            return uint32_t(b0);

        auto isCont= [](unsigned char b) -> bool { return (b & 0xC0u) == 0x80u; };
        integer remaining= src.Length() - idx;

        if((b0 & 0xE0u) == 0xC0u) {
            if(remaining < 2)
                return 0xFFFDu;
            unsigned char b1= readByteAt(idx + 1);
            if(!isCont(b1))
                return 0xFFFDu;
            uint32_t cp= (uint32_t(b0 & 0x1Fu) << 6) | uint32_t(b1 & 0x3Fu);
            if(cp < 0x80u)
                return 0xFFFDu; // overlong
            idx += 1;
            return cp;
        }
        if((b0 & 0xF0u) == 0xE0u) {
            if(remaining < 3)
                return 0xFFFDu;
            unsigned char b1= readByteAt(idx + 1);
            unsigned char b2= readByteAt(idx + 2);
            if(!isCont(b1) || !isCont(b2))
                return 0xFFFDu;
            uint32_t cp= (uint32_t(b0 & 0x0Fu) << 12)
                       | (uint32_t(b1 & 0x3Fu) << 6)
                       |  uint32_t(b2 & 0x3Fu);
            if(cp < 0x800u || (cp >= 0xD800u && cp <= 0xDFFFu))
                return 0xFFFDu; // overlong/surrogate
            idx += 2;
            return cp;
        }
        if((b0 & 0xF8u) == 0xF0u) {
            if(remaining < 4)
                return 0xFFFDu;
            unsigned char b1= readByteAt(idx + 1);
            unsigned char b2= readByteAt(idx + 2);
            unsigned char b3= readByteAt(idx + 3);
            if(!isCont(b1) || !isCont(b2) || !isCont(b3))
                return 0xFFFDu;
            uint32_t cp= (uint32_t(b0 & 0x07u) << 18)
                       | (uint32_t(b1 & 0x3Fu) << 12)
                       | (uint32_t(b2 & 0x3Fu) <<  6)
                       |  uint32_t(b3 & 0x3Fu);
            if(cp < 0x10000u || cp > 0x10FFFFu)
                return 0xFFFDu; // overlong/out of Unicode range
            idx += 3;
            return cp;
        }

        return 0xFFFDu;
    };

    integer actLine= startLine;
    for( const auto& entry : resources ) {
        //file << "// ";
        NString1K line("    \"");
        line._<NC>(entry.Key);
        line._<NC>('"');
        line._<NC>(NFill(' ', int(maxNameLen - line.Length())));

        // write value: We break it into newlines when we find a ’\n'
        line._<NC>(", A_CHAR(\"");
        for( integer i= 0; i < entry.Value.Length(); ++i ) {
            uint32_t cp= decodeUtf8CodePoint(entry.Value, i);
            switch( cp ) {
                case '\\':    line._<NC>("\\\\"); break;
                case '"' :    line._<NC>("\\\"" ); break;
                case '\r':    line._<NC>("\\r"  ); break;
                case '\t':    line._<NC>("\\t"  ); break;
                case '\a':    line._<NC>("\\a"  ); break;
                case '\b':    line._<NC>("\\b"  ); break;
                case '\f':    line._<NC>("\\f"  ); break;
                case '\v':    line._<NC>("\\v"  ); break;
                case '\n': {  line._<NC>("\\n" );
                              if(    line.Length() > maxNameLen + 20
                                  && i < entry.Value.Length() - 1 ) {
                                  line._<NC>("\")\n")
                                      ._<NC>(NFill(' ', int(maxNameLen + 2)))
                                      ._<NC>("A_CHAR(\"");
                              }
                              break;
                           }
                default:
                    if(cp >= 0x20u && cp <= 0x7Eu)
                        line << char(cp);
                    else
                        appendHexEscape(line, cp);
                    break;
        }   }
        line._<NC>("\"),");

        file.At(actLine++).Allocate( file.GetAllocator(), line);
    }

    // write out
    if(!dryrun) {
        errc= file.Write(cppFilePath);
        if(errc != std::errc()) {
            errors.emplace_back( ResourceFileErrors::CPPFileNotWritable,
                                 0,0,0,
                                 PathString(errors.get_allocator().GetAllocator(),
                                 cppFilePath));
    }   }
}

void ListErrors( RCErrorList& errors, alib::Paragraphs& output, const PathString& fileName ) {
    for( const auto& err : errors ) {
        if( err.ErrorCode == ResourceFileErrors::DuplicateResourceName ) {
            output.Add( "Duplicate resource name {}\n"
                        "        First occurrence: {}:{}\n"
                        "   Additional occurrence: {}:{}",
                        err.AdditonalInfo.Unbox<NString>(),
                         fileName, err.LineNo2,
                         fileName, err.LineNo );
            continue;
        }
        NString msg=   err.ErrorCode== ResourceFileErrors::ErrIllegalResourceName     ? "Illegal resource name. In {}:{}:{}"
                     : err.ErrorCode== ResourceFileErrors::ErrNameTooLong             ? "Resource name too long. In {}:{}:{}"
                     : err.ErrorCode== ResourceFileErrors::ErrLineEndWhileInQuotes    ? "Line ended while in quotes. In {}:{}:{}"
                     : err.ErrorCode== ResourceFileErrors::RCFileNotFoundOrAccessible ? "Resource file not found or not accessible: {}{!X}{!X}"
                     : err.ErrorCode== ResourceFileErrors::CPPFileNotFoundOrAccessible? "CPP file not found or not accessible: {}{!X}{!X}"
                     : err.ErrorCode== ResourceFileErrors::CPPFileNotWritable         ? "CPP file not writeable: {}{!X}{!X}"
                     : err.ErrorCode== ResourceFileErrors::CPPFileMissingStartMarker  ? "Missing start marker in the cpp file {}{!X}{!X}"
                     : err.ErrorCode== ResourceFileErrors::CPPFileMissingEndMarker    ? "Missing end marker in the cpp file {}{!X}{!X}"
                     : err.ErrorCode== ResourceFileErrors::CPPFileNotWritable         ? "Error writing the cpp file {}{!X}{!X}"
                     : NULL_NSTRING;
        ALIB_ASSERT_ERROR(msg.IsNotEmpty(), "CAMP/RESCMP", "Unhandled Error: ", int(err.ErrorCode))
        output.Add( msg, err.AdditonalInfo, err.LineNo, err.ColNo );
}   }

bool DevtimeResourceCompiler::Do( const NString&    alibrcFileName,
                                  const NString&    callingFile,
                                  Camp&             campInstance,
                                  const NString&    resourceCategory,
                                  bool              allowReplacements,
                                  const NString&    cppFileName        ) {

    // RC-File: Get absolute path and stats
    Path rcFilePath= Path(alibrcFileName);
    if( !rcFilePath.IsAbsolute() ) {
        rcFilePath.Reset(callingFile);
        rcFilePath.ChangeToParent();
        rcFilePath << DIRECTORY_SEPARATOR << alibrcFileName;
        rcFilePath.MakeCanonical();
    }

    FileStatus rcFileStat (rcFilePath);
    if( rcFileStat.ScanState() != FileStatus::ScanStates::STATS ) {
        ALIB_ERROR( "CAMP/RESCMP", "Could not find resource file \"{}\"", rcFilePath )
        return false;
    }

    // CPP-File: Get an absolute path and stats - only if provided
    Path cppFilePath   = Path(cppFileName);
    if(cppFilePath.IsNotEmpty()) {
        if( !cppFilePath.IsAbsolute() ) {
            cppFilePath.Reset(callingFile);
            cppFilePath.ChangeToParent();
            cppFilePath << DIRECTORY_SEPARATOR << cppFileName;
            cppFilePath.MakeCanonical();
        }

        FileStatus cppFileStat (cppFilePath);
        if( cppFileStat.ScanState() != FileStatus::ScanStates::STATS ) {
            ALIB_ERROR( "CAMP/RESCMP", "Could not find cpp file to write resource-bulkload \"{}\"",
                                       cppFilePath )
            return false;
        }

        // not to update? Note: we add 10ms tolerance. This allows copying/downloading/checking out
        // source files (in the wrong order) to still not re-generate the cpp file. Only if the
        // rc-file seems to have been edited, we continue.
        if(   cppFileStat.MDate() - DateTime::Duration::FromMilliseconds(10)
            >  rcFileStat.MDate() )
            return false;
    }

    // Load resources
    if(!ma) ma= new MonoAllocator(ALIB_DBG("CampRC",) 16);
    ResourceList* orderedList = (*ma)().New<ResourceList>(*ma);
    RCErrorList*  errors      = (*ma)().New<RCErrorList>(*ma);
    LoadResourceFile(rcFilePath, *orderedList, *errors);
    if( !errors->empty() ) {
        Paragraphs output;
        ListErrors(*errors, output, rcFilePath);
        ALIB_ERROR( "CAMP/RESCMP", "Errors while reading resource file \"{}\":\n{}", rcFilePath,
                                   output.Buffer )
        return false;
    }

    // add resources to the given pool
    ResourcePool& pool= campInstance.GetResourcePool();
    for( auto& entry : *orderedList ) {
        ALIB_DBG(bool wasExisting=)
            pool.BootstrapAddOrReplace( resourceCategory, entry.Key, entry.Value );
        ALIB_ASSERT_ERROR(!wasExisting || allowReplacements, "CAMP/RESCMP",
            "Doubly defined resource in {}:{}", rcFilePath, entry.LineNo )
    }

    //  write cpp file
    if( cppFilePath.IsNotEmpty() ) {
        PatchCPPFile(cppFilePath, *orderedList, *errors);
        if( !errors->empty() ) {
            Paragraphs output;
            ListErrors(*errors, output, cppFilePath);
            ALIB_ERROR( "CAMP/RESCMP", "Errors while patching cpp file \"{}\":\n{}", cppFilePath,
                                   output.Buffer )
            return false;
    }   }
    return true;
}


#include "ALib.Lang.CIMethods.H"


} // namespace [alib::camp]


#endif // ALIB_CAMP_RESOURCE_COMPILATION

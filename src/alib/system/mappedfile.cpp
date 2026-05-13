//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software lbicense, see LICENSE.txt)
//##################################################################################################
namespace alib::system {

std::errc MappedFile::Open( const CPathString& path, std::size_t knownSize,
                            bool disableMMap, bool willNeed) {
    Close();
    std::errc errc= std::errc();
    errno = 0;
    
    if (knownSize == (std::numeric_limits<std::size_t>::max)()) {
        #if !defined(_WIN32)
            struct stat st;
            auto result= stat(path, &st);
        #else
            struct _stat64 st;
            auto result= _wstat64(path, &st);
        #endif
        if(result) {
            errc= (errno != 0)  ? std::errc(errno) : std::errc::io_error;
            return errc;
        }
        knownSize = size_t(st.st_size);
    }

    // Empty file is valid; keep backend as ReadAll with empty buffer.
    if (knownSize == 0)
        return errc;

    #if defined(_POSIX_MAPPED_FILES) && _POSIX_MAPPED_FILES > 0
        if (!disableMMap) {
            int fd = ::open(path, O_RDONLY | O_CLOEXEC);
            if (fd != -1) {
                void* p = ::mmap(nullptr, knownSize, PROT_READ, MAP_PRIVATE, fd, 0);
                ::close(fd);
                errno = 0;
                if (p != MAP_FAILED) {
                    mapAddr = p;
                    size    = knownSize;

                    // Best-effort hint: sequential access.
                    // If madvise fails, we ignore it (still valid mapping).
                    if(willNeed)
                        ::madvise(mapAddr, size, MADV_WILLNEED);
                    else
                        ::madvise(mapAddr, size, MADV_SEQUENTIAL);


                    // mmap success
                    return errc;
                }
            } // ::open failed
        } // try mmap
    #else
        (void) willNeed;
    #endif

    // fallback read mode
    #if !defined(_WIN32)
        (void) disableMMap;
        // Portable std::ifstream read-all, size known.
        errno   = 0;
        std::ifstream in(path, std::ios::binary);
        if (!in.is_open())    {
            errc= (errno != 0)  ? std::errc(errno) : std::errc::io_error;
            return errc;
        }

        noMMapBuf.resize(knownSize / sizeof(std::max_align_t) + 1);
        in.read(reinterpret_cast<char*>(noMMapBuf.data()), static_cast<std::streamsize>(knownSize));
        if (!in && !in.eof()) {
            errc= (errno != 0)  ? std::errc(errno) : std::errc::io_error;
            return errc;
        }

        // If file shrank, adjust
        auto got = static_cast<std::size_t>(in.gcount());
        if (got != knownSize) noMMapBuf.resize(got/sizeof(std::max_align_t) + 1);
        size= got;
        return errc;
    #else
        // Windows-specific CreateFile/ReadFile is faster
        (void) disableMMap;
        (void) willNeed;
        errno = 0;
        HANDLE h = CreateFileW( path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr   );
        if (h == INVALID_HANDLE_VALUE) {
            errc = std::errc::io_error;
            return errc;
        }

        noMMapBuf.resize(knownSize/sizeof(std::max_align_t) + 1);

        std::size_t totalRead = 0;
        while (totalRead < knownSize) {
            DWORD chunk = static_cast<DWORD>(
               std::min<std::size_t>(knownSize - totalRead, static_cast<std::size_t>(0x7fffffff)) );

            DWORD bytesRead = 0;
            BOOL ok = ReadFile( h, noMMapBuf.data() + totalRead, chunk, &bytesRead, nullptr );
            if (!ok) {
                CloseHandle(h);
                errc = std::errc::io_error;
                return errc;
            }
            if (bytesRead == 0)
                break; // EOF / file shrank
            totalRead += static_cast<std::size_t>(bytesRead);
        }
        CloseHandle(h);
        if (totalRead != knownSize)
            noMMapBuf.resize(totalRead/sizeof(std::max_align_t) + 1);
        size    = totalRead;
        return errc;
    #endif
}

void MappedFile::Close()                                                                  noexcept {
    #if defined(_POSIX_MAPPED_FILES) && _POSIX_MAPPED_FILES > 0
        if (IsMMap() && size) {
            ::munmap(mapAddr, size);
            mapAddr = nullptr;
        }
    #endif
    noMMapBuf.resize(size= 0);
}



} // namespace [dxl]

// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined(HPP_ALIB_STRINGS_UTIL_TOKENIZER)
    #include "alib/strings/util/tokenizer.hpp"
#endif

#if !defined(HPP_ALIB_SYSTEM_PROCESSINFO)
    #include "alib/system/process.hpp"
#endif

#if !defined(HPP_ALIB_THREADS_THREADLOCK)
    #include "alib/threads/threadlocknr.hpp"
#endif

#if defined(__GLIBCXX__)
    #include <unistd.h>
#elif defined(__APPLE__)
    #include <unistd.h>
    #include <sys/sysctl.h>
    #include <libproc.h>

#elif   defined( _WIN32 )
    #include <direct.h>
#else
    #pragma message ("Unknown Platform in file: " __FILE__ )
#endif

#if !defined(_GLIBCXX_FSTREAM) && !defined(_FSTREAM_)
    #include <fstream>
#endif



namespace aworx { namespace lib { namespace system {

// static instance representing current process
ProcessInfo    ProcessInfo::current;


const ProcessInfo&    ProcessInfo::Current()
{
    static ThreadLockNR   lock;
    if( current.PID == 0 )
    {
        // Own global lock and check if still nulled.
        // (If not, this is a very unlikely parallel access )
        ALIB_LOCK_WITH( lock )
        if ( ProcessInfo::current.PID == 0 )
            ProcessInfo::current.get( 0 );
    }
    return current;
}


#if defined(__GLIBC__) && defined(__unix__)
    bool readProcFile( const NTString& fileName, AString& result  );
    bool readProcFile( const NTString& fileName, AString& result  )
    {
        result.Clear();
        std::ifstream file( fileName.ToCString() );

        std::string buffer;
        getline(file, buffer);

        // spaces are replaced with '\0'. Revert this.
        if (buffer.size() > 2 )
            for( size_t i= 0 ; i < buffer.size() -2 ; ++i )
                if ( buffer[i] == '\0' )
                    buffer[i]= ' ';

        result.Clear()._( buffer.c_str() );
        file.close();
        return true;
    }

    bool ProcessInfo::getStatField( int fieldNo, AString& target)
    {
        target.Clear();
        Tokenizer tknzr( Stat, ' ');
        bool result= true;
        while ( --fieldNo >= 0 && (result= tknzr.HasNext()) )
            tknzr.Next();

        target._( tknzr.Next() );
        return result;
    }

    bool ProcessInfo::get( uinteger pid )
    {
        // use current thread if no PID given
        uinteger newPID= 0;
        if ( pid == 0 )
        {
            auto np= getpid();
            if(np > 0 )
                newPID= static_cast<uinteger>( np );
        }
        else
            newPID= pid;

        if ( newPID == 0 )
            return false;


        PID= newPID;

        // cmdline, stat from proc
        NString64 procDir("/proc/");  procDir._<false>( PID )._( '/' );
        integer    procPathLen= procDir.Length();
        {
            // read things
            procDir << "cmdline"; readProcFile( procDir,  CmdLine  );  procDir.SetLength(procPathLen);
            procDir << "stat";    readProcFile( procDir,  Stat     );  procDir.SetLength(procPathLen);
        }

        getStatField( 3, Name ); // PPID
        PPID= static_cast<uinteger>( Name.ParseInt() );
        getStatField( 1, Name );
        ALIB_ASSERT_ERROR(                 Name.IsEmpty()
                                ||   (     Name.Length() >= 2
                                        && Name.CharAtStart<false>()=='('
                                        && Name.CharAtEnd  <false>()==')' ),
                                ASTR("Error reading process Info") )

        if ( Name.CharAtEnd  () == ')' ) Name.DeleteEnd  <false>( 1 );
        if ( Name.CharAtStart() == '(' ) Name.DeleteStart<false>( 1 );
        getStatField( 2, StatState );
        getStatField( 4, StatPGRP );

        // get executable path and name
        ExecFileName._();
        ExecFilePath._();

        procDir << ASTR("exe");
            char buffer[2048];
            ALIB_STD_TO_NARROW_TSTRING( procDir, nProcDir)
            ssize_t length= readlink( nProcDir.ToCString(), buffer, 2048 );
        procDir.SetLength(procPathLen);

        if( length > 0 )
        {
            ExecFilePath.Append( buffer, length );
            integer idx= ExecFilePath.LastIndexOf( '/' );
            ALIB_ASSERT_ERROR( idx>= 0,
                               (String256() << "Executable path does not contain directory separator character: "
                                            << ExecFilePath ).ToCString() );
            ExecFileName._( ExecFilePath, idx + 1 );
            ExecFilePath.SetLength( idx );
        }
        else
        {
            // obviously no rights to read the link. We use the process name
            ExecFileName._( Name );
        }
        return true;
    }

#elif defined (__APPLE__)

    bool ProcessInfo::get( uinteger pid )
    {
        PID= PPID= 0;
        if( pid == 0 )
            pid= static_cast<uinteger>( getpid() );

        struct proc_bsdinfo proc;
        int st = proc_pidinfo( static_cast<int>(pid), PROC_PIDTBSDINFO, 0, &proc, PROC_PIDTBSDINFO_SIZE);
        if (st != PROC_PIDTBSDINFO_SIZE)
            return false;

        // pid and parent pid
        PID=    pid;
        PPID=   static_cast<uinteger>( proc.pbi_ppid );

        // get name
        Name._(reinterpret_cast<const char*>( proc.pbi_comm ) );

        // get executable filename and path
        {
            char pathbuf[PROC_PIDPATHINFO_MAXSIZE];

            if ( proc_pidpath (static_cast<int>( PID ), pathbuf, PROC_PIDPATHINFO_MAXSIZE) > 0 )
            {
                ExecFilePath._( reinterpret_cast<const char*>( pathbuf ) );
                integer sepPos= ExecFilePath.LastIndexOf( '/' );
                if( sepPos > 0 )
                {
                    ExecFileName._(ExecFilePath, sepPos + 1 );
                    ExecFilePath.SetLength( sepPos );
                }
            }
        }

        return true;
    }

#elif defined (_WIN32)

    bool ProcessInfo::get( uinteger pid )
    {
        // get pid
        if (pid  != 0 )
            return false;

        DWORD wPID= GetCurrentProcessId();
        PID = (uinteger)  wPID;


        // get command line
        CmdLine._()._( GetCommandLine() );

        // get executable filename and path
        ExecFileName._();
        ExecFilePath._();
        Name._();


        char buf[MAX_PATH];
        GetModuleFileNameA( NULL, buf, MAX_PATH );
        ExecFilePath._()._( (const char*) buf );
        integer idx= ExecFilePath.LastIndexOf( '\\' );
        ALIB_ASSERT_ERROR( idx>= 0,
                            String256() << "Executable path does not contain directory separator character: "
                                        << ExecFilePath );
        Name= ExecFileName._( ExecFilePath, idx + 1 );
        ExecFilePath.SetLength( idx );

        // get console title
        STARTUPINFOA startupInfo;
        GetStartupInfoA( &startupInfo );
        ConsoleTitle.Clear()._( startupInfo.lpTitle );

        return true;
    }

#else
    #pragma message ("Unknown Platform in file: " __FILE__ )
#endif



}}}// namespace [aworx::lib::system]


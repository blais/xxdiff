/* -*- c-file-style: "xxdiff" -*- */
/******************************************************************************\
 * $RCSfile$
 *
 * Copyright (C) 1999-2003  Martin Blais <blais@furius.ca>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ******************************************************************************/

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <util.h>
#include <exceptions.h>
#include <main.h>

#include <qstring.h>
#include <qtextstream.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qdatetime.h>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#ifndef WINDOWS
#  include <sys/wait.h>
#  include <sys/time.h>
#  include <unistd.h>
#else
#  include <io.h>
#  include <time.h>
#  include <winsock.h>
//#  include <process.h> // for spawn()

#define pipe _pipe
#define popen _popen
#define pclose _pclose

#endif

#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <fstream>

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

bool installSigChldHandler(
   void (*sigChldHandler)(int)
)
{
// Disabled crud that doesn't work (it works only the first time, the second
// time around the handler is called immediately upon setting up the handler).
   (void)sigChldHandler;
#ifdef DISABLED_ENABLED /* always false */
   XX_ASSERT( sigChldHandler != 0 );

   XX_TRACE( "Installing SIGCHLD handler." );

   // sigset_t spm_o;
   // sigprocmask( SIG_NOP, 0, &spm_o );
   // XX_TRACE( "is SIGCHLD member=" <<
   //           sigismember( &spm_o, SIGCHLD ) );
   // sigemptyset( &spm_o );
   // sigaddset( &spm_o, SIGCHLD );
   // sigprocmask( SIG_BLOCK, &spm_o, 0 );

   //
   // Register a SIGCHLD handler.
   //
   // Note: under IRIX (untested with others), SA_NOCLDWAIT will not
   // allow a redo diff to work. I don't know why.

   struct sigaction sa;
   sa.sa_flags = /*SA_SIGINFO | */SA_RESTART |
      SA_RESETHAND | SA_NOCLDWAIT | SA_NOCLDSTOP;
   sa.sa_handler = sigChldHandler;
   sigset_t ss;
   sigemptyset( &ss );
   sa.sa_mask = ss;
   //sa.sa_sigaction = 0; don't clear sa_sigaction for nothing...
   // sa_handler and sa_sigaction may be sharing an union.
   if ( ::sigaction( SIGCHLD, &sa, 0 ) != 0 ) {
      // Ignore error.
      XX_TRACE( "Error calling sigaction." );
      return false;
   }
#endif
   return true;
}

//------------------------------------------------------------------------------
//
// Returns permissions from QFileInfo in human readable format
// (needs capacity of 12 chars in buf), returns the pointer to the buf for
// convenience.
//
char* qfiToHPerm( char* buf, const QFileInfo& qfi )
{
    char* p = buf;
    // we only support dir option
    *p++ = qfi.isDir() ? 'd' : '-';
    *p++ = qfi.permission( QFileInfo::ReadUser )   ? 'r' : '-';
    *p++ = qfi.permission( QFileInfo::WriteUser )  ? 'w' : '-';
    *p++ = qfi.permission( QFileInfo::ExeUser )    ? 'x' : '-';
    *p++ = qfi.permission( QFileInfo::ReadGroup )  ? 'r' : '-';
    *p++ = qfi.permission( QFileInfo::WriteGroup ) ? 'w' : '-';
    *p++ = qfi.permission( QFileInfo::ExeGroup )   ? 'x' : '-';
    *p++ = qfi.permission( QFileInfo::ReadOther )  ? 'r' : '-';
    *p++ = qfi.permission( QFileInfo::WriteOther ) ? 'w' : '-';
    *p++ = qfi.permission( QFileInfo::ExeOther )   ? 'x' : '-';
    *p = 0;
    return buf;
}

//------------------------------------------------------------------------------
//
// Returns permissions from QFileInfo in octal format.
//
int qfiToPerm( const QFileInfo& qfi )
{
    // This is not nice, but there's no direct way in qfi...
    // also, stat only reports 3 decimals wether it's a file or whatever
    int rv = 0;
    if ( qfi.permission( QFileInfo::ReadUser ) )
        rv += 400;
    if ( qfi.permission( QFileInfo::WriteUser ) )
        rv += 200;
    if ( qfi.permission( QFileInfo::ExeUser ) )
        rv += 100;
    if ( qfi.permission( QFileInfo::ReadGroup ) )
        rv += 40;
    if ( qfi.permission( QFileInfo::WriteGroup ) )
        rv += 20;
    if ( qfi.permission( QFileInfo::ExeGroup ) )
        rv += 10;
    if ( qfi.permission( QFileInfo::ReadOther ) )
        rv += 4;
    if ( qfi.permission( QFileInfo::WriteOther ) )
        rv += 2;
    if ( qfi.permission( QFileInfo::ExeOther ) )
        rv += 1;
    return rv;
}

//------------------------------------------------------------------------------
//
void formatPrintFunc(
   QString&         target,
   char*            pformat,
   char             m,
   const QFileInfo& qfi,
   const QString&   filename,
   bool             newest
)
{
#if (QT_VERSION >= 0x030000)
#define DATEFORMAT Qt::ISODate
#else
#define DATEFORMAT 
#endif

   switch ( m ) {

      case 'q': { // - Print 'NEWEST' if this is the most recently modified
         if ( newest ) {
            target.append( "NEWEST" );
         }
      } break;

      case 'n': { // - File name
         strcat( pformat, "s" );
         QString tmp;
         tmp.sprintf( pformat, filename.latin1() );
         target.append( tmp );
      } break;

      case 'N': { // - Quoted File name with dereference if symbolic link
         strcat( pformat, "s" );
         QString tmp;
         tmp.sprintf( pformat, filename.latin1() );
         if ( qfi.isSymLink() ) {
            tmp.append( "' -> `" );
            tmp.append( qfi.readLink() );
         }
         target.append( "`" );
         target.append( tmp );
         target.append( "'" );
      } break;

      case 'a': { // - Access rights  in octal
         strcat( pformat, "u" );
         QString tmp;
         tmp.sprintf( pformat, qfiToPerm( qfi ) );
         target.append( tmp );
      } break;

      case 'A': { // - Access rights in human readable form
         strcat( pformat, "s" );
         QString tmp;
         char tbuf[11];
         tmp.sprintf( pformat, qfiToHPerm( tbuf,qfi ) );
         target.append( tmp );
      } break;

      case 'F': { // - File type
         strcat( pformat, "s" );
         QString tmp;
         tmp.sprintf( pformat, ( qfi.isFile()?"Regular File":
                                 ( qfi.isDir()?"Directory":
                                   ( qfi.isSymLink()?"Symbolic Link":
                                     "Unknown" ) ) ) );
         target.append( tmp );
      } break;

      case 'U': { // - User name of owner
         strcat( pformat, "s" );
         QString tmp;
         tmp.sprintf( pformat,qfi.owner().ascii() );
         target.append( tmp );
      } break;

      case 'u': { // - User ID of owner
         strcat( pformat, "u" );
         QString tmp;
         tmp.sprintf( pformat,qfi.ownerId() );
         target.append( tmp );
      } break;

      case 'G': { // - Group name of owner
         strcat( pformat, "s" );
         QString tmp;
         tmp.sprintf( pformat,qfi.group().ascii() );
         target.append( tmp );
      } break;

      case 'g': { // - Group ID of owner
         strcat( pformat, "u" );
         QString tmp;
         tmp.sprintf( pformat,qfi.groupId() );
         target.append( tmp );
      } break;

      case 's': { // - Total size, in bytes
         strcat( pformat, "lu" );
         QString tmp;
         tmp.sprintf( pformat,qfi.size() );
         target.append( tmp );
      } break;

      case 'X': { // - Time of last access as seconds since Epoch
         strcat( pformat, "d" );
         QString tmp;
         tmp.sprintf( pformat, XxUtil::toTime_t( qfi.lastRead() ) );
         target.append( tmp );
      } break;

      case 'x': { // -  Time of  last  access
         strcat( pformat, "s" );
         QString tmp;
         // It's not the exact same as stat( 2 ) does, but this is ISO 8601
         // and stat uses some weird syntax of it's own.
         tmp.sprintf( pformat,
                      qfi.lastRead().toString( DATEFORMAT ).ascii() );
         target.append( tmp );
      } break;

      case 'Y': { // - Time of last modification as seconds since Epoch
         strcat( pformat, "d" );
         QString tmp;
         tmp.sprintf( pformat, XxUtil::toTime_t( qfi.lastModified() ) );
         target.append( tmp );
      } break;

      case 'y': { // - Time of last modification
         strcat( pformat, "s" );
         QString tmp;
         // It's not the exact same as stat( 2 ) does, but this is ISO 8601
         // and stat uses some weird syntax of it's own.
         tmp.sprintf( pformat,
                      qfi.lastModified().toString( DATEFORMAT ).ascii() );
         target.append( tmp );
      } break;

      default: {
         throw XxUsageError(
            XX_EXC_PARAMS,
            QString().sprintf( "unknown %%-directive %c\n", m )
         );
      }
   }
}

//------------------------------------------------------------------------------
//
void formatPrintFunc(
   QString& target,
   char*    pformat,
   char     m,
   bool     newest
)
{
   // sensible defaults to stdin
   switch ( m ) {

      case 'q': { // - Print 'NEWEST' if this is the most recently modified
         if ( newest ) {
            target.append( "NEWEST" );
         }
      } break;

      case 'n': { // - File name
         target.append( "( stdin )" );
      } break;

      case 'N': { // - Quoted File name with dereference if symbolic link
         target.append( "`( stdin )'" );
      } break;

      case 'a': { // - Access rights in octal
         target.append( "0777" );
      } break;

      case 'A': { // - Access rights in human readable form
         target.append( "-rwxrwxrwx" );
      } break;

      case 'F': { // - File type
         target.append( "Standard Input" );
      } break;

      case 'U': { // - User name of owner
         target.append( "user" );
      } break;

      case 'u': { // - User ID of owner
         target.append( "X" );
      } break;

      case 'G': { // - Group name of owner
         target.append( "X" );
      } break;

      case 'g': { // - Group ID of owner
         target.append( "X" );
      } break;

      case 's': { // - Total size, in bytes
         target.append( "X" );
      } break;

      case 'X': { // - Time of last access as seconds since Epoch
         strcat( pformat, "d" );
         QString tmp;
         tmp.sprintf( pformat, 
                      XxUtil::toTime_t( QDateTime::currentDateTime() ) );
         target.append( tmp );
      } break;

      case 'x': { // -  Time of  last  access
         strcat( pformat, "s" );
         QString tmp;
         // It's not the exact same as stat( 2 ) does, but this is ISO 8601
         // and stat uses some weird syntax of it's own.
         tmp.sprintf(
            pformat,
            ( QDateTime::currentDateTime() ).toString( DATEFORMAT ).ascii()
         );
         target.append( tmp );
      } break;

      case 'Y': { // - Time of last modification as seconds since Epoch
         strcat( pformat, "d" );
         QString tmp;
         tmp.sprintf( pformat, 
                      XxUtil::toTime_t( QDateTime::currentDateTime() ) );
         target.append( tmp );
      } break;

      case 'y': { // - Time of last modification
         strcat( pformat, "s" );
         QString tmp;
         // It's not the exact same as stat( 2 ) does, but this is ISO 8601
         // and stat uses some weird syntax of it's own.
         tmp.sprintf(
            pformat,
            ( QDateTime::currentDateTime() ).toString( DATEFORMAT ).ascii() );
         target.append( tmp );
      } break;

      default: {
         throw XxUsageError(
            XX_EXC_PARAMS,
            QString().sprintf( "unknown %%-directive %c\n", m )
         );
      }

   }
}

}

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

//------------------------------------------------------------------------------
//
void XxUtil::copyToFile( FILE* fin, FILE* fout )
{
   while ( !feof(fin) ) {
      char buffer[BUFSIZ];
      size_t nitems = fread( buffer, 1, BUFSIZ, fin );
      if ( fwrite( buffer, 1, nitems, fout ) != nitems ) {
         break;
      }
   }
   if ( ferror( fin ) || ferror( fout ) ) {
      fclose(fout);
      throw XxIoError( XX_EXC_PARAMS, "Error writing temporary file." );
   }
}

//------------------------------------------------------------------------------
//
int XxUtil::copyFile( const QString& src, const QString& dest )
{
   QString cmd = QString("cp '") + src + QString("' '") + dest + QString("'");

   FILE* f = popen( cmd.latin1(), "r" );
   int r = pclose( f );
   return r;
}

//------------------------------------------------------------------------------
//
int XxUtil::removeFile( const QString& src )
{
   XX_ASSERT( !src.isEmpty() );
   return unlink( src.latin1() );
}

//------------------------------------------------------------------------------
//
bool XxUtil::testFile(
   const QString& filename,
   bool           testAscii,
   bool&          isDirectory
)
{
    QFileInfo finfo( filename );
    return testFile( filename, finfo, testAscii, isDirectory );
}

//------------------------------------------------------------------------------
//
bool XxUtil::testFile(
   const QString&   filename,
   const QFileInfo& finfo,
   bool             testAscii,
   bool&            isDirectory
)
{
   if ( !finfo.exists() || !finfo.isReadable() ) {
      QString s;
      QTextOStream oss( & s );
      oss << "Cannot access file: " << filename;
      throw XxIoError( XX_EXC_PARAMS, s );
   }

   // Don't do this anymore, we want to be able to read /dev/null and symlinks!
   // Actually, that check was still working with symlinks, but according to the
   // documentation it shouldn't have.  We don't need this error.
   //
   // // Check if file is a regular file or a directory.
   // if ( !( finfo.isFile() || finfo.isDir() ) ) {
   //    QString os;
   //    QTextOStream oss( &os );
   //    oss << "Error: not an ordinary file or a directory";
   //    throw XxIoError( XX_EXC_PARAMS, os );
   // }

   // If file is a directory.
   if ( finfo.isDir() ) {
      isDirectory = true;
      return true;
   }
   else {
      isDirectory = false;
   }

   // Make sure file is not binary, we don't handle binary files.
   if ( testAscii ) {
      if ( !isAsciiText( filename ) ) {
         QString os;
         QTextOStream oss( &os );
         oss << "Error: file is not a text file";
         throw XxIoError( XX_EXC_PARAMS, os );
      }
   }

   return true;
}

//------------------------------------------------------------------------------
//
bool XxUtil::isAsciiText( const QString& filename )
{
   int fd, bytes, i;
   char buffer[1024];

   fd = open( filename.latin1(), O_RDONLY );
   bytes = read( fd, (void *)buffer, 1024 );
   close( fd );

   // Here's our current looser definition of an ascii file: on the first line
   // of the file there must not be any non-ascii character.
   for ( i = 0; i < bytes; i++ ) {
      if ( buffer[i] == '\n' ) {
         break;
      }
      if ( !isascii( buffer[i] ) ) {
         return false;
      }
   }
   return true;
}

//------------------------------------------------------------------------------
//
int XxUtil::spawnCommand(
   const char** argv,
   FILE** outf,
   FILE** errf,
   void (*sigChldHandler)(int),
   const char* cstdin
)
{
   XX_ASSERT( argv );

#ifndef WINDOWS

   int pipe_fds_in[2];
   if ( cstdin ) {
      // Open the pipe.
      if ( pipe( pipe_fds_in ) == -1 ) {
         throw XxIoError( XX_EXC_PARAMS );
      }
   }

   int pipe_fds_out[2];
   if ( outf ) {
      // Open the pipe.
      if ( pipe( pipe_fds_out ) == -1 ) {
         throw XxIoError( XX_EXC_PARAMS );
      }
   }

   int pipe_fds_err[2];
   if ( errf ) {
      if ( pipe( pipe_fds_err ) == -1 ) {
         throw XxIoError( XX_EXC_PARAMS );
      }
   }

   int pid = fork();
   switch ( pid ) {
      case 0: { // the child

         /*
          * pipe standard input into the pipe
          */
         if ( cstdin ) {
            if ( dup2( pipe_fds_in[0], fileno( stdin ) ) == -1 ) {
               throw XxIoError( XX_EXC_PARAMS );
            }
            close( pipe_fds_in[1] );
         }

         /*
          * redirect standard output and standard error into the pipe
          */
         if ( outf ) {
            /*close( fileno( stdout ) );*/
            /*if ( dup( pipe_fds_out[1] ) == -1 ) {*/
            if ( dup2( pipe_fds_out[1], fileno( stdout ) ) == -1 ) {
               throw XxIoError( XX_EXC_PARAMS );
            }
            close( pipe_fds_out[0] );
         }

         if ( errf ) {
            /*close( fileno( stderr ) );*/
            /*if ( dup( pipe_fds_err[1] ) == -1 ) {*/
            if ( dup2( pipe_fds_err[1], fileno( stderr ) ) == -1 ) {
               throw XxIoError( XX_EXC_PARAMS );
            }
            close( pipe_fds_err[0] );
         }

         if ( execvp( argv[0], const_cast<char**>(argv) ) == -1 ) {
            // Send parent some output telling it we couldn't exec.
            QString errs;
            {
               QTextOStream errss( &errs );
               errss << "Cannot exec process " << argv[0] << endl << flush;
            }
            fwrite( errs.latin1(), errs.length(), 1, stderr );
            fwrite( "\n", 1, 1, stderr );

            exit( 1 );
         }

         // Unreached.

      } break;

      case -1: { // fork error
         throw XxIoError( XX_EXC_PARAMS );
      }

      default: { // the parent

         if ( sigChldHandler ) {
            if ( installSigChldHandler( sigChldHandler ) == false ) {
               return -2;
            }
         }

         /*
          * we must close this in the parent or else the close of the
          * writer end of the pipe in the child will not cause an EOF
          * condition for the reader
          */
         if ( cstdin ) {
            close( pipe_fds_in[0] );
         }
         if ( outf ) {
            close( pipe_fds_out[1] );
         }
         if ( errf ) {
            close( pipe_fds_err[1] );
         }

         /*
          * if requested, write into stdin of the subprocess.
          */
         if ( cstdin ) {
            FILE* inf = fdopen( pipe_fds_in[1], "w" );

            unsigned int len = strlen( cstdin );
            // XX_TRACE( "writing " << len << " bytes" );
            if ( fwrite( cstdin, sizeof(char), len, inf ) != len ) {
               throw XxIoError( XX_EXC_PARAMS );
            }

            if ( fclose( inf ) != 0 ) {
               throw XxIoError( XX_EXC_PARAMS );
            }
         }

         /*
          * return the reader side of the pipe as a stdio stream.
          */
         if ( outf ) {
            *outf = fdopen( pipe_fds_out[0], "r" );
            if ( !*outf ) {
               throw XxIoError( XX_EXC_PARAMS );
            }
         }
         if ( errf ) {
            *errf = fdopen( pipe_fds_err[0], "r" );
            if ( !*errf ) {
               throw XxIoError( XX_EXC_PARAMS );
            }
         }
      }
   }

#else

   int pid = -1;

   QString command;
   const char** arg;
   for ( arg = argv; *arg != 0; ++arg ) {
      command += QString(*arg) + QString(" ");
   }
   XX_TRACE( "Command: " << command.latin1() );

//    /*
//     * N**xed up version with Windows calls. Consider yourself lucky if this
//     * works even just once.
//     */

//    if ( _spawnvp( _P_NOWAIT | _P_DETACH, argv[0], const_cast<char**>(argv) ) != 0 ) {
//       // Send parent some output telling it we couldn't exec.
//       QString errs;
//       {
//          QTextOStream errss( &errs );
//          errss << "Error spawning process " << argv[0] << endl << flush;
//       }
//       fwrite( errs.latin1(), errs.length(), 1, stderr );
//       fwrite( "\n", 1, 1, stderr );

//       exit( 1 );
//    }

   FILE* outputf;
   FILE* errorf;

   /*
    * Run command so that it writes its output to a pipe. Open this pipe with
    * read text attribute so that we can read it like a text file.
    */
   if( (outputf = popen( command.latin1(), "rt" )) == NULL ) {
      throw XxIoError( XX_EXC_PARAMS );
   }
   errorf = stderr;

#if KEPT_FOR_HISTORY
   /*
    * Read pipe until end of file. End of file indicates that outputf closed its
    * standard out (probably meaning it terminated).
    */
   while( !feof( chkdsk ) )
   {
      if( fgets( psBuffer, 128, chkdsk ) != NULL )
         printf( psBuffer );
   }
#endif

   /* Close pipe and print return value of outputf. */
   *outf = outputf;
   *errf = errorf;
//   printf( "\nProcess returned %d\n", pclose( outputf ) );
// FIXME todo, check result value as well

#endif

   // Not reached.
   return pid;
}

//------------------------------------------------------------------------------
//
int XxUtil::spawnCommand(
   const char** argv,
   void (*sigChldHandler)(int)
)
{
   return spawnCommand( argv, 0, 0, sigChldHandler );
}

//------------------------------------------------------------------------------
//
int XxUtil::interruptibleSystem( const QString& command )
{
   // This is code for an interruptible system() call as shown as suggested in
   // GNU libc/Linux system(3) man page.

#ifndef WINDOWS

   if ( command.isEmpty() ) {
      return 1;
   }
   int pid = fork();
   if ( pid == -1 ) {
      return -1;
   }
   if ( pid == 0 ) {
      char* argv[4];
      argv[0] = "sh";
      argv[1] = "-c";
      argv[2] = const_cast<char*>( command.latin1() );
      argv[3] = 0;
      execve( "/bin/sh", argv, environ );
      exit( 127 );
   }
   do {
      int status;
      if ( waitpid( pid, &status, 0 ) == -1) {
         if ( errno != EINTR ) {
            return -1;
         }
      }
      else {
         return status;
      }
   } while(1);
#endif
   return -1;
}

//------------------------------------------------------------------------------
//
long XxUtil::getCurrentTime()
{
#ifndef WINDOWS
   struct timeval tv;
   struct timezone tz;
   gettimeofday( &tv, &tz );
   return long( ( tv.tv_sec % 100 ) * 1e6 + tv.tv_usec );
#else
   return long( timeGetTime() );
#endif
}

//------------------------------------------------------------------------------
//
void XxUtil::printTime( std::ostream& os, long time )
{
   long tv_sec = long( time / 1e6 );
   long tv_usec = time - long( tv_sec * 1e6 );
   os << "sec.usec=" << tv_sec << "." << tv_usec << std::endl;
}

//------------------------------------------------------------------------------
//
int XxUtil::splitArgs(
   const QString&     command,
   const QStringList& filenames,
   const char**&      out_args
)
{
    // Titles aren't needed that often...
    const QString * titles[3] = { NULL, NULL, NULL };
    return splitArgs(command,titles,filenames,out_args);
}


//------------------------------------------------------------------------------
//
int XxUtil::splitArgs(
   const QString&     command,
   const QString *    titles[3],
   const QStringList& filenames,
   const char**&      out_args
)
{
   /*
    * split up args passed in into an argument vector for the execvp
    * system call.  this works for an unlimited number of arguments,
    * but fails to do any quote processing.  arguments with embedded
    * spaces will break this.
    */

   QStringList args = QStringList::split( QRegExp( "\\s" ), command );
   args += filenames;
   int argc = 0;
   // It doesn't hurt to reserve space for 3 slots even though we may
   // not need them all.
   const char** argv =
      (const char**) malloc( sizeof(char*) * (args.count() + 1 + 3) );
   for ( QStringList::Iterator it = args.begin();
         it != args.end();
         ++it ) {

      argv[argc++] = strdup( (*it).latin1() );
   }
   for (int i=0; i<3; i++)
      if (titles[i]) {
         argv[argc++] = strdup( titles[i]->latin1() );
      }
   argv[argc] = 0;

//#define ANAL_DEBUGGING
#ifdef ANAL_DEBUGGING
   std::ofstream ofs( "/tmp/diff_args" );
   ofs << " ARGS ------------------------------" << std::endl;
   const char** ppargs = argv;
   while ( *ppargs != 0 ) {
      ofs << *ppargs << std::endl;
      ++ppargs;
   }
   ofs << " -----------------------------------" << std::endl;
   ofs.close();
#endif

   out_args = argv;
   return argc;
}

//------------------------------------------------------------------------------
//
void XxUtil::freeArgs( const char**& out_args )
{
   if ( out_args ) {
      for ( const char** dargv = out_args; *dargv; ++dargv ) {
         free( const_cast<char*>( *dargv ) );
      }
      free( out_args );
      out_args = 0; // reset it, just in case.
   }
}

//------------------------------------------------------------------------------
//
QString XxUtil::removeClearCaseExt( const QString& filename )
{
   // Remove ClearCase extended syntax if it is there.
   int bpos = filename.find( "@@" );
   QString cleanname;
   if ( bpos != -1 ) {
      cleanname = filename.mid( 0, bpos );
   }
   else {
      cleanname = filename;
   }
   return cleanname;
}

//------------------------------------------------------------------------------
//
bool XxUtil::formatFilename(
   QString&         target,
   const QString&   masterformat,
   const QFileInfo& finfo,
   bool             isStdin,
   const QString&   filename,
   bool             newest
)
{
   // FIXME:
   // - This code is not exception safe (i.e. called function can throw).
   // - I'm not sure that the buffer sizes are always valid, to review.
   //
   // Including anyway for now, because I absolutely don't have time right now.

   char* format = strdup( masterformat.latin1() );
   char* dest = (char*) malloc( strlen( format ) + 1 );
   char* b = format;
   target = "";

   // Below code is an adapted extract from GNU Coreutils stat
   // along with the bug fix discovered while doing that.
   // Idea is that the %-sequences get changed in formatPrintFunc
   // for sprintf. Also the format string gets null terminators 
   // here and there, that's why we need two copies of the masterformat.
   while ( b ) {

      char *p = strchr( b, '%' );
      if ( p != 0 ) {
         size_t len;
         *p++ = '\0';
         target.append( b );

         len = strspn( p, "#-+.I 0123456789" );
         dest[0] = '%';
         memcpy( dest + 1, p, len );
         dest[ 1 + len ] = 0;
         p += len;

         b = p + 1;
         switch (*p ) {
            case '\0':
               b = 0;
            case '%':
               target.append( "%" );
               break;
            default: {
               if ( isStdin ) {
                  formatPrintFunc( target, dest, *p, newest );
               }
               else {
                  formatPrintFunc( target, dest, *p, finfo, filename, newest );
               }
            } break;
         }
      } else {
         target.append( b );
         b = 0;
      }
   }

   free( format );
   free( dest );
   return true;
}

//------------------------------------------------------------------------------
//
unsigned int XxUtil::toTime_t( 
#if (QT_VERSION >= 0x030100)
   const QDateTime& t
#else
   const QDateTime& /*t*/
#endif
)
{
#if (QT_VERSION >= 0x030100)
   return t.toTime_t();
#else
   return 0; // Note: we could hack something by hand.
#endif
}

//------------------------------------------------------------------------------
//
QString XxUtil::escapeChars( const QString& format )
{
   QString newFormat = format;

   newFormat.replace( "\n", "\\n" );
   newFormat.replace( "\r", "\\r" );
   newFormat.replace( "\"", "\\\"" );

   return newFormat;
}

//------------------------------------------------------------------------------
//
QString XxUtil::unescapeChars( const QString& format )
{
   QString newFormat = format;
   uint ix = 0;

   while ( ix < newFormat.length() ) {
       int found = newFormat.find( QChar( '\\' ), ix );
       if ( found < 0 )
	   break;
       // use at() in case found+1 is past the end of the string
       QChar escapedChar = newFormat.at( found+1 );
       switch( escapedChar ) {
       case 'n':
	   newFormat = newFormat.replace( found, 2, QChar( '\n' ) );
	   break;
       case 'r':
	   newFormat = newFormat.replace( found, 2, QChar( '\r' ) );
	   break;
       default:
	   newFormat = newFormat.remove( found, 1 );
	   break;
       }
       ix = found + 1;
   }
   
   return newFormat;
}

//------------------------------------------------------------------------------
//
FILE* XxUtil::tempfile( char* tmplate )
{
   // Open the temporary file.
   FILE *ftmp;
#ifndef WINDOWS
   int tfd = mkstemp( tmplate );
   if ( ( ftmp = ::fdopen( tfd, "w" ) ) == NULL ) {
#else
   mktemp( tmplate );
   if ( ( ftmp = ::fopen( tmplate, "w" ) ) == NULL ) {
#endif
      throw XxIoError( XX_EXC_PARAMS, 
                       "Error opening temporary file." );
   }
   return ftmp;
}


XX_NAMESPACE_END

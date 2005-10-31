/* -*- c-file-style: "xxdiff" -*- */
/******************************************************************************\
 * $RCSfile$
 *
 * Copyright (C) 1999-2002  Martin Blais <blais@iro.umontreal.ca>
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
   bool     testAscii,
   bool&          isDirectory
)
{
   // Check for access.
   QFileInfo finfo( filename );

   if ( !finfo.exists() || !finfo.isReadable() ) {
      QString s;
      QTextOStream oss( & s );
      oss << "Cannot access file: " << filename;
      throw XxIoError( XX_EXC_PARAMS, s );
   }

   // Check if file is a regular file or a directory.  
   if ( !( finfo.isFile() || finfo.isDir() ) ) {
      QString os;
      QTextOStream oss( &os );
      oss << "Error: not an ordinary file or a directory";
      throw XxIoError( XX_EXC_PARAMS, os );
   }

   // If file is a directory.
   if ( finfo.isDir() ) {
      isDirectory = true;
      return true;
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
void XxUtil::spawnCommand(
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

   switch ( fork() ) {
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
               return;
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

#if 0 
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
   return;
}

//------------------------------------------------------------------------------
//
void XxUtil::spawnCommand( 
   const char** argv,
   void (*sigChldHandler)(int)
)
{
   spawnCommand( argv, 0, 0, sigChldHandler );
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
   /* 
    * split up args passed in into an argument vector for the execvp
    * system call.  this works for an unlimited number of arguments,
    * but fails to do any quote processing.  arguments with embedded
    * spaces will break this.
    */
   
   QStringList args = QStringList::split( QRegExp( "\\s" ), command );
   args += filenames;
   int argc = 0;
   const char** argv =
      (const char**) malloc( sizeof(char*) * (args.count() + 1) );
   for ( QStringList::Iterator it = args.begin(); 
         it != args.end();
         ++it ) {
      
      argv[argc++] = strdup( (*it).latin1() );
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

XX_NAMESPACE_END

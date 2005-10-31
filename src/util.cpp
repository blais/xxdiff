/******************************************************************************\
 * $Id: util.cpp 298 2001-10-23 03:18:14Z blais $
 * $Date: 2001-10-22 23:18:14 -0400 (Mon, 22 Oct 2001) $
 *
 * Copyright (C) 1999-2001  Martin Blais <blais@iro.umontreal.ca>
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
 *****************************************************************************/

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <util.h>
#include <exceptions.h>
#include <main.h>

#include <qstring.h>
#include <qtextstream.h>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef WINDOWS
#  include <sys/wait.h>
#  include <sys/time.h>
#  include <unistd.h>
#  include <libgen.h>
#else
#  include <io.h>
#  include <time.h>
#  include <winsock.h>

#define popen _popen
#define pclose _pclose
#define access _access
#define R_OK 04

#endif

#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

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
bool XxUtil::isDirectory( const QString& filename )
{
   struct stat buf;
   if ( stat( filename.latin1(), &buf ) != 0 ) {
      QString os;
      QTextOStream oss( &os );
      oss << "Cannot open file: " << filename;
      throw XxIoError( XX_EXC_PARAMS, os );
   }
   return ((buf.st_mode&S_IFMT) == S_IFDIR);
}


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
   QString cmd = QString("cp ") + src + QString(" ") + dest;

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
   const bool     testAscii,
   bool&          isDirectory
)
{
   struct stat buf;
   
   // Check for access.
   if ( access( filename.latin1(), R_OK ) != 0 ) {
      QString s;
      QTextOStream oss( & s );
      oss << "Cannot access file: " << filename;
      throw XxIoError( XX_EXC_PARAMS, s );
   }

   // Stat file.
   if ( stat( filename.latin1(), &buf ) != 0 ) {
      QString os;
      QTextOStream oss( &os );
      oss << "Cannot open file: " << filename;
      throw XxIoError( XX_EXC_PARAMS, os );
   }

   // Check if file is a regular file or a directory.  
   if ( !( ((buf.st_mode&S_IFMT) == S_IFREG) || 
           ((buf.st_mode&S_IFMT) == S_IFDIR) ) ) {
      QString os;
      QTextOStream oss( &os );
      oss << "Error: not an ordinary file or a directory";
      throw XxIoError( XX_EXC_PARAMS, os );
   }

   // If file is a directory.
   if ( ((buf.st_mode&S_IFMT) == S_IFDIR) ) {
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
   
   // Here's our current looser definition of an ascii file: on the file line of
   // the file there must not be any non-ascii character.
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
bool XxUtil::spawnCommand( 
   const char** argv,
   void (*sigChldHandler)(int)
)
{
   XX_ASSERT( argv );

#ifndef WINDOWS

   switch ( fork() ) {
      case 0: { // the child

         if ( execvp( argv[0], (char**)argv ) == -1 ) {
            exit( 1 );
         }

         // Unreached.

      } break;
      
      case -1: { // fork error
         throw XxIoError( XX_EXC_PARAMS );
      }

      default: {
         if ( sigChldHandler ) {
            if ( installSigChldHandler( sigChldHandler ) == false ) {
               return false;
            }
         }
      }
   }
#endif
   
   // The parent. Return. Forget about it.
   return true;
}

//------------------------------------------------------------------------------
//
void XxUtil::spawnCommandWithOutput(
   const char** argv,
   FILE*& outf,
   FILE*& errf,
   void (*sigChldHandler)(int)
)
{
   XX_ASSERT( argv );

   outf = errf = 0; // initialize.
#ifndef WINDOWS

   // Open the pipe.
   int pipe_fds_out[2];
   if ( pipe( pipe_fds_out ) == -1 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }

   int pipe_fds_err[2];
   if ( pipe( pipe_fds_err ) == -1 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }

   switch ( fork() ) {
      case 0: { // the child

         /* 
          * redirect standard output and standard error into the pipe
          */
         close( fileno( stdout ) );
         if ( dup( pipe_fds_out[1] ) == -1 ) {
            throw XxIoError( XX_EXC_PARAMS );
         }
         close( fileno( stderr ) );
         if ( dup( pipe_fds_err[1] ) == -1 ) {
            throw XxIoError( XX_EXC_PARAMS );
         }

         close( pipe_fds_out[0] );
         close( pipe_fds_err[0] );
         
         if ( execvp( argv[0], const_cast<char**>(argv) ) == -1 ) {
            // Send parent some output telling it we couldn't exec.
            QString errs;
            {
               QTextOStream errss( &errs );
               errss << "Cannot exec process " << argv[0] << endl << flush;
            }
            fwrite( errs.latin1(), errs.length(), 1, stderr );
            fwrite( "\n", 1, 1, stderr );
//             fwrite( stderr, pipe_fds_err[0], errs.latin1(), errs.length() );
//             fwrite( stderr, pipe_fds_err[0], "\n", 1 );
            
//             close( pipe_fds_out[0] );
//             close( pipe_fds_err[1] );

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
         close( pipe_fds_out[1] );
         close( pipe_fds_err[1] );

         /* 
          * return the reader side of the pipe as a stdio stream
          */
         outf = fdopen( pipe_fds_out[0], "r" );
         errf = fdopen( pipe_fds_err[0], "r" );
         if ( !outf || !errf ) {
            throw XxIoError( XX_EXC_PARAMS );
         }
      }
   }
#endif

   // Not reached.
   return;
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
QString XxUtil::baseName( const QString& str )
{
#ifndef WINDOWS
   QString bn( ::basename( const_cast<char*>( str.latin1() ) ) );
   return bn;
#else
   const DWORD BUFFER_LEN = 1024;
   char  buffer[BUFFER_LEN];
   char* name;
   
   if ( ::GetFullPathName( str.latin1(),
                           BUFFER_LEN,
                           buffer,
                           &name ) == 0 ) {
      throw XxInternalError( XX_EXC_PARAMS );
   }

   if ( name == 0 ) {
      return QString();
   }

   return QString( name );
#endif
}

//------------------------------------------------------------------------------
//
int XxUtil::splitArgs( 
   const QString& command,
   const char**&  out_args
)
{
   /* 
    * split up args passed in into an argument vector for the execvp
    * system call.  this works for an unlimited number of arguments,
    * but fails to do any quote processing.  arguments with embedded
    * spaces will break this.
    */
   
   int argc = 0;
   const int BLOCKSIZE = 10;
   int count = BLOCKSIZE;
   const char** argv = (const char**) malloc( sizeof(char*) * count );
   
   // Make a copy of the args string because strtok is broken under
   // Linux, it modifies the first argument (see BUGS section in man
   // page of strtok).
   char* cargs = strdup( command.latin1() );
         
   char* ptr;
   for ( ptr = strtok( cargs, " \t" ); 
         ptr; 
         ptr = strtok( 0, " \t" ) ) {
            
      if ( argc >= count ) {
         count += BLOCKSIZE;
         argv = (const char**) realloc( argv, sizeof(char*) * count );
      }
            
      argv[argc++] = strdup( ptr );
   }
   free( cargs );
         
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

XX_NAMESPACE_END


/******************************************************************************\
 * $Id: util.cpp 64 2001-03-11 01:06:13Z  $
 * $Date: 2001-03-10 20:06:13 -0500 (Sat, 10 Mar 2001) $
 *
 * Copyright (C) 1999, 2000  Martin Blais <blais@iro.umontreal.ca>
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

#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

//------------------------------------------------------------------------------
//
const char** splitArgs( 
   const char* command,
   const char* args[]
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
   char* cargs = strdup( command );
         
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
         
   const char** pargs = args;
   while ( *pargs != 0 ) {
      if ( argc >= count ) {
         count += BLOCKSIZE;
         argv = (const char**) realloc( argv, sizeof(char*) * count );
      }

      argv[argc++] = *pargs;
      pargs++;
   }
   argv[argc++] = 0;

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

   return argv;
}

}

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

//------------------------------------------------------------------------------
//
bool XxUtil::isDirectory( const char* filename )
{
   struct stat buf;
   if ( stat( filename, &buf ) != 0 ) {
      std::ostringstream oss;
      oss << "Cannot stat file: " << filename << std::ends;
      throw new XxIoError( oss.str().c_str() );
   }
   return S_ISDIR( buf.st_mode );
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
      throw new XxIoError( "Error writing temporary file." );
   }
}

//------------------------------------------------------------------------------
//
bool XxUtil::testFile( const char* filename, bool& isDirectory )
{
   struct stat buf;
   
   // Check for access.
   if ( access( filename, R_OK ) != 0 ) {
      std::ostringstream oss;
      oss << "Cannot access file: " << filename << std::ends;
      throw new XxIoError( oss.str().c_str() );
   }

   // Stat file.
   if ( stat( filename, &buf ) != 0 ) {
      std::ostringstream oss;
      oss << "Cannot open file: " << filename << std::ends;
      throw new XxIoError( oss.str().c_str() );
   }

   // Check if file is a regular file or a directory.  
   if ( !( S_ISREG( buf.st_mode ) || S_ISDIR( buf.st_mode ) ) ) {
      std::ostringstream oss;
      oss << "Error: not an ordinary file or a directory";
      throw new XxIoError( oss.str().c_str() );
   }

   // If file is a directory.
   if ( S_ISDIR( buf.st_mode ) ) {
      isDirectory = true;
      return true;
   }
   
   // Make sure file is not binary, we don't handle binary files.
   if ( !isAsciiText( filename ) ) {
      std::ostringstream oss;
      oss << "Error: file is not a text file";
      throw new XxIoError( oss.str().c_str() );
   }

   return true;
}

//------------------------------------------------------------------------------
//
bool XxUtil::isAsciiText( const char *filename )
{
   int fd, bytes, i;
   char buffer[1024];
   
   fd = open( filename, O_RDONLY );
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
   const char* command,
   const char* args[],
   void (*sigChldHandler)(int)
)
{
   XX_ASSERT( command != 0 );

   switch ( fork() ) {
      case 0: { // the child

         const char** argv = splitArgs( command, args );

         if ( execvp( argv[0], (char**)argv ) == -1 ) {
            exit( 1 );
         }

         // Unreached.

      } break;
      
      case -1: { // fork error
         throw new XxIoError;
      }

      default: {
         if ( sigaction != 0 ) {
            XX_TRACE( "Installing SIGCHLD handler." );
//              sigset_t spm_o;
//              sigprocmask( SIG_NOP, 0, &spm_o );
//              XX_TRACE( "is SIGCHLD member=" << sigismember( &spm_o, SIGCHLD ) );

//              sigemptyset( &spm_o );
//              sigaddset( &spm_o, SIGCHLD );
//              sigprocmask( SIG_BLOCK, &spm_o, 0 );
// FIXME remove

            //
            // Register a SIGCHLD handler.
            //
            // Note: under IRIX (untested with others), SA_NOCLDWAIT will not
            // allow a redo diff to work. I don't know why.

            struct sigaction sa;
            sa.sa_flags = SA_SIGINFO | SA_RESETHAND /*| SA_NOCLDWAIT*/;
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
         }
      }
   }
   
   // The parent. Return. Forget about it.
   return true;
}

//------------------------------------------------------------------------------
//
FILE* XxUtil::spawnCommandWithOutput( 
   const char* command,
   const char* args[]
)
{
   XX_ASSERT( command != 0 );

   // Open the pipe.
   int pipe_fds[2];
   if ( pipe( pipe_fds ) == -1 ) {
      throw new XxIoError;
   }

   switch ( fork() ) {
      case 0: { // the child

         /* 
          * redirect standard output and standard error into the pipe
          */
         close( fileno( stdout ) );
         if ( dup( pipe_fds[1] ) == -1 ) {
            throw new XxIoError;
         }
         close( fileno( stderr ) );
         if ( dup( pipe_fds[1] ) == -1 ) {
            throw new XxIoError;
         }

         close( pipe_fds[0] );

         const char** argv = splitArgs( command, args );

         if ( execvp( argv[0], (char**)argv ) == -1 ) {
            exit( 1 );
         }

         // Unreached.

      } break;
      
      case -1: { // fork error
         throw new XxIoError;
      }

      default: { // the parent

         /* 
          * we must close this in the parent or else the close of the 
          * writer end of the pipe in the child will not cause an EOF 
          * condition for the reader
          */
         close( pipe_fds[1] );

         /* 
          * return the reader side of the pipe as a stdio stream
          */
         return fdopen( pipe_fds[0], "r" );
      }
   }

   // Not reached.
   return 0;
}

//------------------------------------------------------------------------------
//
int XxUtil::interruptibleSystem( const char* command )
{
   // This is code for an interruptible system() call as shown as suggested in
   // GNU libc/Linux system(3) man page.

   if ( command == 0 ) {
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
      argv[2] = (char*)command;
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
   return -1;
}

XX_NAMESPACE_END

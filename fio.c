#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/syscall.h>

#include "fio.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#define MAX_PATH 4096

FILE* aml_fio_open_for_writing(char *fn, char *source_file, int source_line) {
   FILE *f;
   if(!(f=fopen(fn,"w"))) {
      fprintf(stderr,"%s:%d: Error opening \"%s\" for writing\n",source_file,source_line,fn);
      exit(1);
   } return f;
}

FILE* aml_fio_open_for_reading(char *fn, char *source_file, int source_line) {
   FILE *f;
   if(!(f=fopen(fn,"r"))) {
      fprintf(stderr,"%s:%d: Error opening \"%s\" for reading\n",source_file,source_line,fn);
      exit(1);
   } return f;
}

/* Gets a line from a file, skips comments, returns 0 at EOF, else returns 1 */
int aml_fio_get_line(FILE *f, char *lineBuffer, int maxLength) {
   int c; int pos=0;

   while((c=fgetc(f))!=EOF) {
      switch(c) {
         case '#':
         while((c=fgetc(f))!=EOF) {
            if(c=='\n') break;
         }
         if(pos) {
            if(pos<maxLength-1)
               lineBuffer[pos++] = '\n';
            lineBuffer[pos]   = '\0';
            return 1;
         }
         break;

         case '\n':
         if(pos) {
            if(pos<maxLength-1)
               lineBuffer[pos++] = '\n';
            lineBuffer[pos]   = '\0';
            return 1;
         } else {
            // This line only contains a '\n' or is a comment so ignore
         }
         break;

         default:
         if(pos<maxLength-1)
            lineBuffer[pos++] = c;
         else {
            lineBuffer[pos] = '\0';
            return 1;
         }
         break;
      }
   }
   
   if(pos) {
      if(pos<maxLength-1)
         lineBuffer[pos++] = '\n';
      lineBuffer[pos]   = '\0';
      return 1;
   }
   return 0;
}

/* Converts a file to a stream of characters, strips comments in the process. */
char* aml_fio_file_to_char_stream(FILE *f) {
   char *lineBuffer = (char*)malloc(MAX_LINE_LENGTH*sizeof(char));
   char *charStream = NULL, *charStreamPointer = NULL;
   int charStreamSize = 0, stringLen = 0;

   /* Read line by line */
   while(aml_fio_get_line(f,lineBuffer,MAX_LINE_LENGTH)) {
      stringLen  = strlen(lineBuffer)+1;
      charStream = realloc(charStream,(charStreamSize+stringLen)*sizeof(char));
      charStreamPointer = charStream + charStreamSize;
      charStreamSize += stringLen-1;
      strncpy(charStreamPointer,lineBuffer,stringLen);
   }
   if(charStreamSize==0) {
      free(lineBuffer);
      return NULL;
   }

   charStream[charStreamSize-1] = '\0';
   free(lineBuffer);
   return charStream;
}

FILE* aml_fio_open_file_and_strip_comments(char *fN) {
   FILE *f;
   
   // Open file and convert to character stream with comments stripped
   if(!(f=fopen(fN,"r"))) {
      fprintf(stderr,"%s:%d: Error opening \"%s\" for reading\n",
         __FILE__,__LINE__,fN);
      exit(1);
   }
   return aml_fio_strip_comments(f);
}

FILE* aml_fio_strip_comments(FILE *f) {
   char *charStream = aml_fio_file_to_char_stream(f);
   if(!charStream) return NULL;
   fclose(f);
   // Create a temporary file
   char *tempFN = (char*)malloc(12*sizeof(char));
   sprintf(tempFN,"temp.XXXXXX");
   mode_t oldMode = umask(077);
   f = fdopen(mkstemp(tempFN),"w");
   (void) umask(oldMode);
   // Write the char stream to this file
   fwrite(charStream,1,strlen(charStream),f);
   // Close this file and open for reading
   fclose(f); f = fopen(tempFN,"r");
   // Unlink the file on disk so temporary file dissapears after fclose
   unlink(tempFN);
   // Free temporary memory
   free(charStream); free(tempFN);
   return f;
}

void aml_fio_dirwalk_files_only_recurse(char *dir, void (*fcn)(char *)) {
   char name[MAX_PATH];
   struct dirent *dp;
   struct stat stbuf;
   DIR *dfd;

   // open directory
   if( (dfd = opendir(dir)) == NULL) {
      fprintf(stderr,"dirwalk: can't open %s\n",dir);
      exit(1);
   }

   // call function fcn on each entry
   while( (dp = readdir(dfd)) != NULL) {
      // do not call function on "." or ".."
      if(!strcmp(dp->d_name,".") || !strcmp(dp->d_name,"..")) continue;

      // check that current directory name + filename does not exceed MAX_PATH
      if(strlen(dir)+strlen(dp->d_name)+2 > sizeof(name)) {
         fprintf(stderr,"dirwalk: name %s/%s to long\n",dir,dp->d_name);
         exit(1);
      } else {
         // otherwise check if file is directory, if not call function
         sprintf(name,"%s/%s",dir,dp->d_name);

         if(stat(name,&stbuf)==-1) {
            fprintf(stderr,"fileops: cannot access %s\n",name);
            exit(1);
         }
         // check if file is regular, and call function if so
         if((stbuf.st_mode & S_IFMT) == S_IFREG) (*fcn)(name);
         else if((stbuf.st_mode & S_IFMT) == S_IFDIR)
            aml_fio_dirwalk_files_only_recurse(name,fcn);
      }
   }
   closedir(dfd);
}

void aml_fio_dirwalk_files_only_with_args(
   char *dir,
   void (*fcn)(char *, void **args),
   void **args
   ) {

   // local
   char name[MAX_PATH];
   struct dirent *dp;
   struct stat stbuf;
   DIR *dfd;

   // open directory
   if( (dfd = opendir(dir)) == NULL) {
      fprintf(stderr,"dirwalk: can't open %s\n",dir);
      exit(1);
   }

   // call function fcn on each entry
   while( (dp = readdir(dfd)) != NULL) {
      // do not call function on "." or ".."
      if(!strcmp(dp->d_name,".") || !strcmp(dp->d_name,"..")) continue;

      // check that current directory name + filename does not exceed MAX_PATH
      if(strlen(dir)+strlen(dp->d_name)+2 > sizeof(name)) {
         fprintf(stderr,"dirwalk: name %s/%s to long\n",dir,dp->d_name);
         exit(1);
      } else {
         // otherwise check if file is directory, if not call function
         sprintf(name,"%s/%s",dir,dp->d_name);

         if(stat(name,&stbuf)==-1) {
            fprintf(stderr,"fileops: cannot access %s\n",name);
            exit(1);
         }
         // check if file is regular, and call function if so
         if((stbuf.st_mode & S_IFMT) == S_IFREG) (*fcn)(name,args);
      }
   }
   closedir(dfd);
}

void aml_fio_dirwalk_files_only(char *dir, void (*fcn)(char *)) {
   char name[MAX_PATH];
   struct dirent *dp;
   struct stat stbuf;
   DIR *dfd;

   // open directory
   if( (dfd = opendir(dir)) == NULL) {
      fprintf(stderr,"dirwalk: can't open %s\n",dir);
      exit(1);
   }

   // call function fcn on each entry
   while( (dp = readdir(dfd)) != NULL) {
      // do not call function on "." or ".."
      if(!strcmp(dp->d_name,".") || !strcmp(dp->d_name,"..")) continue;

      // check that current directory name + filename does not exceed MAX_PATH
      if(strlen(dir)+strlen(dp->d_name)+2 > sizeof(name)) {
         fprintf(stderr,"dirwalk: name %s/%s to long\n",dir,dp->d_name);
         exit(1);
      } else {
         // otherwise check if file is directory, if not call function
         sprintf(name,"%s/%s",dir,dp->d_name);

         if(stat(name,&stbuf)==-1) {
            fprintf(stderr,"fileops: cannot access %s\n",name);
            exit(1);
         }
         // check if file is regular, and call function if so
         if((stbuf.st_mode & S_IFMT) == S_IFREG) (*fcn)(name);
      }
   }
   closedir(dfd);
}

// directory transversal routine, courtesy of K&R2
void aml_fio_dirwalk(char *dir, void (*fcn)(char *)) {
   char name[MAX_PATH];
   struct dirent *dp;
   DIR *dfd;

   // open directory
   if( (dfd = opendir(dir)) == NULL) {
      fprintf(stderr,"dirwalk: can't open %s\n",dir);
      exit(1);
   }

   // call function fcn on each entry
   while( (dp = readdir(dfd)) != NULL) {
      // do not call function on "." or ".."
      if(!strcmp(dp->d_name,".") || !strcmp(dp->d_name,"..")) continue;

      // check that current directory name + filename does not exceed MAX_PATH
      if(strlen(dir)+strlen(dp->d_name)+2 > sizeof(name)) {
         fprintf(stderr,"dirwalk: name %s/%s to long\n",dir,dp->d_name);
         exit(1);
      } else {
         // otherwise call the passed function with the dir entry
         sprintf(name,"%s/%s",dir,dp->d_name);
         (*fcn)(name);
      }
   }
   closedir(dfd);
}

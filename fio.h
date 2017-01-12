#ifndef AML_FIO_H 
#define AML_FIO_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100000
// XXX This MAX_LINE_LENGTH is a problem because if something exceeds it
// the system can fail, so somehow deal with it dynamically
int aml_fio_get_line(FILE *f, char *lineBuffer, int maxLength);
char* aml_fio_file_to_char_stream(FILE *f);
FILE* aml_fio_strip_comments(FILE *f);
FILE* aml_fio_open_file_and_strip_comments(char *fN);
FILE* aml_fio_open_for_reading(char *fn, char *source_file, int source_line);
FILE* aml_fio_open_for_writing(char *fn, char *source_file, int source_line);
void aml_fio_dirwalk(char *dir, void (*fcn)(char *));
void aml_fio_dirwalk_files_only(char *dir, void (*fcn)(char *));
void aml_fio_dirwalk_files_only_with_args(
   char *dir,
   void (*fcn)(char *, void **args),
   void **args
);
void aml_fio_dirwalk_files_only_recurse(char *dir, void (*fcn)(char *));
#endif

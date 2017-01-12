#ifndef AML_ARRAY_H
#define AML_ARRAY_H
#include <stdio.h>
/*******************************************************************************
 * Array handling functions.                                                   *
 * Mills, A                                      Sat Aug 30 11:12:38 BST 2003  *
 ******************************************************************************/

// INTEGER ARRAY MANIPULATION

// FIXED SIZE CREATION
/**
 * Potato chip generator
 * \param egg egg size
 * \param dog dog size
 */
int*    aml_array_int_1d_new(int s1, int val); 
int**   aml_array_int_2d_new(int s1, int s2, int val); 
int***  aml_array_int_3d_new(int s1, int s2, int s3, int val); 
int**** aml_array_int_4d_new(int s1, int s2, int s3, int s4, int val);

// INTEGER ARRAY VARIABLE SIZE CREATION
int** aml_array_int_2d_variable_2nd_new(int s1, int *s2, int val);
int**** aml_array_int_4d_variable_2nd_new(int s1, int *s2, int s3, int s4, int val);

// INTEGER ARRAY READING
int*    aml_array_int_1d_load(FILE *file, int s1); 
int**   aml_array_int_2d_load(FILE *file, int s1, int s2); 
int***  aml_array_int_3d_load(FILE *file, int s1, int s2, int s3); 
int**** aml_array_int_4d_load(FILE *file, int s1, int s2, int s3, int s4); 
int** aml_array_int_2d_variable_2nd_load(FILE *file, int s1, int *s2);
int**** aml_array_int_4d_variable_2nd_load(FILE *file, int s1, int *s2, int s3, int s4);

// INTEGER ARRAY WRITING
void aml_array_int_1d_save(FILE *file, int *arr,    int s1); 
void aml_array_int_2d_save(FILE *file, int **arr,   int s1, int s2); 
void aml_array_int_3d_save(FILE *file, int ***arr,  int s1, int s2, int s3); 
void aml_array_int_4d_save(FILE *file, int ****arr, int s1, int s2, int s3, int s4); 
void aml_array_int_2d_variable_2nd_save(FILE *file, int **arr, int s1, int *s2);
void aml_array_int_4d_variable_2nd_save(FILE *file, int ****arr, int s1, int *s2, int s3, int s4);

// INTEGER ARRAY COPYING
void aml_array_int_1d_copy(int     *dst, int     *src, int s1);
void aml_array_int_2d_copy(int    **dst, int    **src, int s1, int s2);
void aml_array_int_3d_copy(int   ***dst, int   ***src, int s1, int s2, int s3);
void aml_array_int_4d_copy(int  ****dst, int  ****src, int s1, int s2, int s3, int s4);
void aml_array_int_5d_copy(int *****dst, int *****src, int s1, int s2, int s3, int s4, int s5);

// DOUBLE ARRAY CREATION

// FIXED SIZE CREATION
double*    aml_array_dbl_1d_new(int s1, double val); 
double**   aml_array_dbl_2d_new(int s1, int s2, double val); 
double***  aml_array_dbl_3d_new(int s1, int s2, int s3, double val); 
double**** aml_array_dbl_4d_new(int s1, int s2, int s3, int s4, double val);

// FIXED SIZE FILLING
void aml_array_dbl_1d_fill(double *arr, int s1, double val);
void aml_array_dbl_2d_fill(double **arr, int s1, int s2, double val);

// DOUBLE ARRAY VARIABLE SIZE CREATION
double** aml_array_dbl_2d_variable_2nd_new(int s1, int *s2, double val);
double*** aml_array_dbl_3d_variable_2nd_new(int s1, int *s2, int s3, double val);
double*** aml_array_dbl_3d_variable_3rd_new(int s1, int s2, int *s3, double val);
double**** aml_array_dbl_4d_variable_2nd_new(int s1, int *s2, int s3, int s4, double val);
double**** aml_array_dbl_4d_variable_4th_new(int s1, int s2, int s3, int *s4, double val);
double**** aml_array_dbl_4d_variable_2nd_and_4th(int s1, int *s2, int s3, int *s4, double val);

// DOUBLE ARRAY WRITING
void aml_array_dbl_1d_save(FILE *file, double *arr,    int s1); 
void aml_array_dbl_2d_save(FILE *file, double **arr,   int s1, int s2); 
void aml_array_dbl_3d_save(FILE *file, double ***arr,  int s1, int s2, int s3); 
void aml_array_dbl_4d_save(FILE *file, double ****arr, int s1, int s2, int s3, int s4); 
void aml_array_dbl_2d_variable_2nd_save(FILE *file, double **arr, int s1, int *s2);
void aml_array_dbl_4d_variable_2nd_save(FILE *file, double ****arr, int s1, int *s2, int s3, int s4);

// DOUBLE ARRAY READING
double*    aml_array_dbl_1d_load(FILE *file, int s1); 
double**   aml_array_dbl_2d_load(FILE *file, int s1, int s2); 
double***  aml_array_dbl_3d_load(FILE *file, int s1, int s2, int s3); 
double**** aml_array_dbl_4d_load(FILE *file, int s1, int s2, int s3, int s4); 
double**   aml_array_dbl_2d_variable_2nd_load(FILE *file, int s1, int *s2);
double**** aml_array_dbl_4d_variable_2nd_load(FILE *file, int s1, int *s2, int s3, int s4);

// DOUBLE ARRAY COPYING
void aml_array_dbl_1d_copy(double    *dst, double    *src, int s1);
void aml_array_dbl_2d_copy(double   **dst, double   **src, int s1, int s2);
void aml_array_dbl_3d_copy(double  ***dst, double  ***src, int s1, int s2, int s3);
void aml_array_dbl_4d_copy(double ****dst, double ****src, int s1, int s2, int s3, int s4);
void aml_array_dbl_5d_copy(double *****dst, double *****src, int s1, int s2, int s3, int s4, int s5);
void aml_array_dbl_2d_variable_2nd_copy(double **dst, double **src, int s1, int *s2);

// ARRAY DEALLOCATION
void aml_array_int_1d_free(int *arr);
void aml_array_int_2d_free(int **arr, int s1);
void aml_array_int_3d_free(int ***arr, int s1, int s2);
void aml_array_int_4d_free(int ****arr, int s1, int s2, int s3);
void aml_array_int_2d_variable_2nd_free(int **arr, int s1, int *s2);

void aml_array_dbl_1d_free(double *arr);
void aml_array_dbl_2d_free(double **arr, int s1);
void aml_array_dbl_3d_free(double ***arr, int s1, int s2);
void aml_array_dbl_4d_free(double ****arr, int s1, int s2, int s3);
void aml_array_dbl_2d_variable_2nd_free(double **arr, int s1, int *s2);
void aml_array_dbl_3d_variable_2nd_free(double ***arr, int s1, int *s2);
void aml_array_dbl_4d_variable_2nd_free(double ****arr, int s1, int *s2, int s3);
#endif

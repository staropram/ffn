#include <stdio.h>
#include <stdlib.h>
#include "array.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/*******************************************************************************
 * Array handling functions.                                                   *
 * Mills, A                                      Sat Aug 30 11:12:38 BST 2003  *
 ******************************************************************************/

// INTEGER ARRAY MANIPULATION
// FIXED SIZE CREATION
// IAIF = IntArrayInitFunction

/**
 * Allocates a 1D integer array.
 * \param s1 The length of the array.
 * \param val The value that array elements should be initialized to.
 */
int* aml_array_int_1d_new(int s1, int val) {
   int i; int *arr = (int *)malloc(s1*sizeof(int));
   if(s1) {
      for(i=0; i<s1; i++) arr[i] = val;
   } else {
      arr = NULL;
   }
   return arr;
}

int** aml_array_int_2d_new(int s1, int s2, int val) {
   int i,j; int **arr = (int **)malloc(s1*sizeof(int *));
   for(i=0; i<s1; i++) {
      arr[i] = (int *)malloc(s2*sizeof(int));
      for(j=0; j<s2; j++) arr[i][j] = val;
   } return arr;
}

int*** aml_array_int_3d_new(int s1, int s2, int s3, int val) {
   int i,j,k; 
   int ***arr = (int ***)malloc(s1*sizeof(int **));
   for(i=0; i<s1; i++) {
      arr[i] = (int **)malloc(s2*sizeof(int *));
      for(j=0; j<s2; j++) {
         arr[i][j] = (int *)malloc(s3*sizeof(int));
         for(k=0; k<s3; k++) arr[i][j][k] = val;
      }
   } return arr;
}

int**** aml_array_int_4d_new(int s1, int s2, int s3, int s4, int val) {
   int i,j,k,l;
   int ****arr = (int ****)malloc(s1*sizeof(int ***));
   for(i=0; i<s1; i++) {
      arr[i] = (int ***)malloc(s2*sizeof(int **));
      for(j=0; j<s2; j++) {
         arr[i][j] = (int **)malloc(s3*sizeof(int *));
         for(k=0; k<s3; k++) {
            arr[i][j][k] = (int *)malloc(s4*sizeof(int));
            for(l=0; l<s4; l++) arr[i][j][k][l] = val;
         }
      }
   } return arr;
}

// INTEGER ARRAY VARIABLE SIZE CREATION
int** aml_array_int_2d_variable_2nd_new(int s1, int *s2, int val) {
   int i,j; int **arr = (int **)malloc(s1*sizeof(int *));
   for(i=0; i<s1; i++) {
      arr[i] = (int *)malloc(s2[i]*sizeof(int));
      for(j=0; j<s2[i]; j++) arr[i][j] = val;
   } return arr;
}

int**** aml_array_int_4d_variable_2nd_new(int s1, int *s2, int s3, int s4, int val) {
   int i,j,k,l;
   int ****arr = (int ****)malloc(s1*sizeof(int ***));
   for(i=0; i<s1; i++) {
      arr[i] = (int ***)malloc(s2[i]*sizeof(int **));
      for(j=0; j<s2[i]; j++) {
         arr[i][j] = (int **)malloc(s3*sizeof(int *));
         for(k=0; k<s3; k++) {
            arr[i][j][k] = (int *)malloc(s4*sizeof(int));
            for(l=0; l<s4; l++) arr[i][j][k][l] = val;
         }
      }
   } return arr;
}

// INTEGER ARRAY READING
int* aml_array_int_1d_load(FILE *file, int s1) {
   int i; int *arr = aml_array_int_1d_new(s1,0);
   for(i=0; i<s1; i++)
      fscanf(file,"%d",&arr[i]);
   return arr;
}

int** aml_array_int_2d_load(FILE *file, int s1, int s2) {
   int i,j;
   int **arr = aml_array_int_2d_new(s1,s2,0);
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++)
         fscanf(file,"%d",&arr[i][j]);
   return arr;
}

int*** aml_array_int_3d_load(FILE *file, int s1, int s2, int s3) {
   int i,j,k;
   int ***arr = aml_array_int_3d_new(s1,s2,s3,0);
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++)
         for(k=0; k<s3; k++)
            fscanf(file,"%d",&arr[i][j][k]);
   return arr;
}

int**** aml_array_int_4d_load(FILE *file, int s1, int s2, int s3, int s4) {
   int ****arr = aml_array_int_4d_new(s1,s2,s3,s4,0);
   int i,j,k,l;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++)
         for(k=0; k<s3; k++)
            for(l=0; l<s4; l++)
               fscanf(file, "%d ", &arr[i][j][k][l]);
   return arr;
}

int** aml_array_int_2d_variable_2nd_load(FILE *file, int s1, int *s2) {
   int i,j;
   int **arr = aml_array_int_2d_variable_2nd_new(s1,s2,0);
   for(i=0; i<s1; i++)
      for(j=0; j<s2[i]; j++)
         fscanf(file, "%d ", &arr[i][j]);
   return arr;
}

int**** aml_array_int_4d_variable_2nd_load(FILE *file, int s1, int *s2, int s3, int s4) {
   int ****arr = aml_array_int_4d_variable_2nd_new(s1,s2,s3,s4,0);

   int i,j,k,l;
   for(i=0; i<s1; i++)
      for(j=0; j<s2[i]; j++)
         for(k=0; k<s3; k++)
            for(l=0; l<s4; l++)
               fscanf(file, "%d ", &arr[i][j][k][l]);
   return arr;
}

// INTEGER ARRAY WRITING
void aml_array_int_1d_save(FILE *file, int *arr, int s1) {
   int i; for(i=0; i<s1; i++) fprintf(file,"%d ",arr[i]);
}

void aml_array_int_2d_save(FILE *file, int **arr, int s1, int s2) {
   int i,j;
   for(i=0; i<s1; i++) {
      for(j=0; j<s2; j++)
         fprintf(file, "%d ", arr[i][j]);
      fprintf(file,"\n");
   }
}

void aml_array_int_3d_save(FILE *file, int ***arr,  int s1, int s2, int s3) {
   int i,j,k;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++)
         for(k=0; k<s3; k++)
            fprintf(file, "%d ", arr[i][j][k]);
}

void aml_array_int_4d_save(FILE *file, int ****arr, int s1, int s2, int s3, int s4) {
   int i,j,k,l;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++)
         for(k=0; k<s3; k++)
            for(l=0; l<s4; l++)
               fprintf(file, "%d ", arr[i][j][k][l]);
}

void aml_array_int_2d_variable_2nd_save(FILE *file, int **arr, int s1, int *s2) {
   int i,j;
   for(i=0; i<s1; i++) {
      for(j=0; j<s2[i]; j++)
         fprintf(file, "%d ", arr[i][j]);
      fprintf(file,"\n");
   }
}

void aml_array_int_4d_variable_2nd_save(FILE *file, int ****arr, int s1, int *s2, int s3, int s4) {
   int i,j,k,l;
   for(i=0; i<s1; i++)
      for(j=0; j<s2[i]; j++)
         for(k=0; k<s3; k++)
            for(l=0; l<s4; l++)
               fprintf(file, "%d ", arr[i][j][k][l]);
}

// INTEGER ARRAY COPYING
void aml_array_int_1d_copy(int *dst, int *src, int s1) {
   int i; for(i=0; i<s1; i++) dst[i] = src[i];
}

void aml_array_int_2d_copy(int **dst, int **src, int s1, int s2) {
   int i,j;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++) dst[i][j] = src[i][j];
}

void aml_array_int_3d_copy(int  ***dst, int  ***src, int s1, int s2, int s3) {
   int i,j,k;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++) 
         for(k=0; k<s3; k++)
            dst[i][j][k] = src[i][j][k];
}

void aml_array_int_4d_copy(int ****dst, int ****src, int s1, int s2, int s3, int s4) {
   int i,j,k,l;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++) 
         for(k=0; k<s3; k++)
            for(l=0; l<s4; l++)
               dst[i][j][k][l] = src[i][j][k][l];
}

void aml_array_int_5d_copy(int *****dst, int *****src, int s1, int s2, int s3, int s4, int s5) {
   int i,j,k,l,m;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++) 
         for(k=0; k<s3; k++)
            for(l=0; l<s4; l++)
               for(m=0; m<s5; m++)
                  dst[i][j][k][l][m] = src[i][j][k][l][m];
}

// DOUBLE ARRAY CREATION

// FIXED SIZE CREATION
double* aml_array_dbl_1d_new(int s1, double val) {
   int i; double *arr = (double *)malloc(s1*sizeof(double));
   for(i=0; i<s1; i++) arr[i] = val;
   return arr;
}

double** aml_array_dbl_2d_new(int s1, int s2, double val) {
   int i,j; double **arr = (double **)malloc(s1*sizeof(double *));
   for(i=0; i<s1; i++) {
      arr[i] = (double *)malloc(s2*sizeof(double));
      for(j=0; j<s2; j++) arr[i][j] = val;
   } return arr;
}

double***  aml_array_dbl_3d_new(int s1, int s2, int s3, double val) {
   int i,j,k; 
   double ***arr = (double ***)malloc(s1*sizeof(double **));
   for(i=0; i<s1; i++) {
      arr[i] = (double **)malloc(s2*sizeof(double *));
      for(j=0; j<s2; j++) {
         arr[i][j] = (double *)malloc(s3*sizeof(double));
         for(k=0; k<s3; k++) arr[i][j][k] = val;
      }
   } return arr;
}

double**** aml_array_dbl_4d_new(int s1, int s2, int s3, int s4, double val) {
   int i,j,k,l;
   double ****arr = (double ****)malloc(s1*sizeof(double ***));
   for(i=0; i<s1; i++) {
      arr[i] = (double ***)malloc(s2*sizeof(double **));
      for(j=0; j<s2; j++) {
         arr[i][j] = (double **)malloc(s3*sizeof(double *));
         for(k=0; k<s3; k++) {
            arr[i][j][k] = (double *)malloc(s4*sizeof(double));
            for(l=0; l<s4; l++) arr[i][j][k][l] = val;
         }
      }
   } return arr;
}

// FIXED SIZE FILLING
void aml_array_dbl_1d_fill(double *arr, int s1, double val) {
   int i;
   for(i=0; i<s1; i++)
      arr[i] = val;
}

void aml_array_dbl_2d_fill(double **arr, int s1, int s2, double val) {
   int i,j;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++)
         arr[i][j] = val;
}

// DOUBLE ARRAY VARIABLE SIZE CREATION
double** aml_array_dbl_2d_variable_2nd_new(int s1, int *s2, double val) {
   int i,j;
   double **arr;
   if(s1) {
      arr = (double **)malloc(s1*sizeof(double *));
      for(i=0; i<s1; i++) {
         if(s2[i]) {
            arr[i] = (double *)malloc(s2[i]*sizeof(double));
            for(j=0; j<s2[i]; j++) arr[i][j] = val;
         } else {
            arr[i] = NULL;
         }
      }
   } else {
      // XXX I really need a backtrace here
      //typedef void (*stupid)(); stupid x = (stupid)(s2[0]); x(); // core
      fprintf(stderr,"%s:%d Attempt to initialise empty array\n",__FILE__,__LINE__);
      exit(1);
   }
   
   return arr;
}

double*** aml_array_dbl_3d_variable_2nd_new(int s1, int *s2, int s3, double val) {
   int i,j,k;
   double ***arr = (double***)malloc(s1*sizeof(double **));
   for(i=0; i<s1; i++) {
      arr[i] = (double **)malloc(s2[i]*sizeof(double *));
      for(j=0; j<s2[i]; j++) {
         arr[i][j] = (double *)malloc(s3*sizeof(double));
         for(k=0; k<s3; k++) arr[i][j][k] = val;
      }
   } return arr;
}

double*** aml_array_dbl_3d_variable_3rd_new(int s1, int s2, int *s3, double val) {
   int i,j,k;
   double ***arr = (double***)malloc(s1*sizeof(double **));
   for(i=0; i<s1; i++) {
      arr[i] = (double **)malloc(s2*sizeof(double *));
      for(j=0; j<s2; j++) {
         arr[i][j] = (double *)malloc(s3[j]*sizeof(double));
         for(k=0; k<s3[j]; k++) arr[i][j][k] = val;
      }
   } return arr;
}

double**** aml_array_dbl_4d_variable_2nd_new(int s1, int *s2, int s3, int s4, double val) {
   int i,j,k,l;
   double ****arr = (double ****)malloc(s1*sizeof(double ***));
   for(i=0; i<s1; i++) {
      arr[i] = (double ***)malloc(s2[i]*sizeof(double **));
      for(j=0; j<s2[i]; j++) {
         arr[i][j] = (double **)malloc(s3*sizeof(double *));
         for(k=0; k<s3; k++) {
            arr[i][j][k] = (double *)malloc(s4*sizeof(double));
            for(l=0; l<s4; l++) arr[i][j][k][l] = val;
         }
      }
   } return arr;
}

double**** aml_array_dbl_4d_variable_4th_new(int s1, int s2, int s3, int *s4, double val) {
   int i,j,k,l;
   double ****arr = (double ****)malloc(s1*sizeof(double ***));
   for(i=0; i<s1; i++) {
      arr[i] = (double ***)malloc(s2*sizeof(double **));
      for(j=0; j<s2; j++) {
         arr[i][j] = (double **)malloc(s3*sizeof(double *));
         for(k=0; k<s3; k++) {
            arr[i][j][k] = (double *)malloc(s4[k]*sizeof(double));
            for(l=0; l<s4[k]; l++) arr[i][j][k][l] = val;
         }
      }
   } return arr;
}

double**** aml_array_dbl_4d_variable_2nd_and_4th(int s1, int *s2, int s3, int *s4, double val) {
   int i,j,k,l;
   double ****arr = (double ****)malloc(s1*sizeof(double ***));
   for(i=0; i<s1; i++) {
      arr[i] = (double ***)malloc(s2[i]*sizeof(double **));
      for(j=0; j<s2[i]; j++) {
         arr[i][j] = (double **)malloc(s3*sizeof(double *));
         for(k=0; k<s3; k++) {
            arr[i][j][k] = (double *)malloc(s4[k]*sizeof(double));
            for(l=0; l<s4[k]; l++) arr[i][j][k][l] = val;
         }
      }
   } return arr;
}

// DOUBLE ARRAY READING
double* aml_array_dbl_1d_load(FILE *file, int s1) {
   int i; double *arr = aml_array_dbl_1d_new(s1,0);
   for(i=0; i<s1; i++)
      fscanf(file,"%lf",&arr[i]);
   return arr;
}

double** aml_array_dbl_2d_load(FILE *file, int s1, int s2) {
   int i,j;
   double **arr = aml_array_dbl_2d_new(s1,s2,0);
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++)
         fscanf(file,"%lf",&arr[i][j]);
   return arr;
}

double*** aml_array_dbl_3d_load(FILE *file, int s1, int s2, int s3) {
   int i,j,k;
   double ***arr = aml_array_dbl_3d_new(s1,s2,s3,0);
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++)
         for(k=0; k<s3; k++)
            fscanf(file,"%lf",&arr[i][j][k]);
   return arr;
}

double**** aml_array_dbl_4d_load(FILE *file, int s1, int s2, int s3, int s4) {
   double ****arr = aml_array_dbl_4d_new(s1,s2,s3,s4,0);
   int i,j,k,l;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++)
         for(k=0; k<s3; k++)
            for(l=0; l<s4; l++)
               fscanf(file,"%lf",&arr[i][j][k][l]);
   return arr;
}

double** aml_array_dbl_2d_variable_2nd_load(FILE *file, int s1, int *s2) {
   int i,j;
   double **arr = aml_array_dbl_2d_variable_2nd_new(s1,s2,0);
   for(i=0; i<s1; i++)
      for(j=0; j<s2[i]; j++)
         fscanf(file,"%lf",&arr[i][j]);
   return arr;
}

double**** aml_array_dbl_4d_variable_2nd_load(FILE *file, int s1, int *s2, int s3, int s4) {
   double ****arr = aml_array_dbl_4d_variable_2nd_new(s1,s2,s3,s4,0);
   int i,j,k,l;
   for(i=0; i<s1; i++)
      for(j=0; j<s2[i]; j++)
         for(k=0; k<s3; k++)
            for(l=0; l<s4; l++)
               fscanf(file,"%lf",&arr[i][j][k][l]);
   return arr;
}

// DOUBLE ARRAY WRITING
void aml_array_dbl_1d_save(FILE *file, double *arr, int s1) {
   int i; for(i=0; i<s1; i++) fprintf(file,"%.20lf ",arr[i]);
}

void aml_array_dbl_2d_save(FILE *file, double **arr, int s1, int s2) {
   int i,j;
   for(i=0; i<s1; i++) {
      for(j=0; j<s2; j++)
         fprintf(file, "%.20lf ", arr[i][j]);
      fprintf(file,"\n");
   }
}

void aml_array_dbl_3d_save(FILE *file, double ***arr, int s1, int s2, int s3) {
   int i,j,k;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++)
         for(k=0; k<s3; k++)
            fprintf(file, "%.20lf ", arr[i][j][k]);
}

void aml_array_dbl_4d_save(FILE *file, double ****arr, int s1, int s2, int s3, int s4) {
   int i,j,k,l;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++)
         for(k=0; k<s3; k++)
            for(l=0; l<s4; l++)
               fprintf(file, "%.20lf ", arr[i][j][k][l]);
}

void aml_array_dbl_2d_variable_2nd_save(FILE *file, double **arr, int s1, int *s2) {
   int i,j;
   for(i=0; i<s1; i++) {
      for(j=0; j<s2[i]; j++)
         fprintf(file, "%.20lf ", arr[i][j]);
      fprintf(file,"\n");
   }
}

void aml_array_dbl_4d_variable_2nd_save(FILE *file, double ****arr, int s1, int *s2, int s3, int s4) {
   int i,j,k,l;
   for(i=0; i<s1; i++)
      for(j=0; j<s2[i]; j++)
         for(k=0; k<s3; k++)
            for(l=0; l<s4; l++)
               fprintf(file, "%.20lf ", arr[i][j][k][l]);
}

// DOUBLE ARRAY COPYING
void aml_array_dbl_1d_copy(double *dst, double *src, int s1) {
   int i; for(i=0; i<s1; i++) dst[i] = src[i];
}

void aml_array_dbl_2d_copy(double **dst, double **src, int s1, int s2) {
   int i,j;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++) dst[i][j] = src[i][j];
}

void aml_array_dbl_3d_copy(double ***dst, double ***src, int s1, int s2, int s3) {
   int i,j,k;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++) 
         for(k=0; k<s3; k++)
            dst[i][j][k] = src[i][j][k];
}

void aml_array_dbl_4d_copy(double ****dst, double ****src, int s1, int s2, int s3, int s4) {
   int i,j,k,l;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++) 
         for(k=0; k<s3; k++)
            for(l=0; l<s4; l++)
               dst[i][j][k][l] = src[i][j][k][l];
}

void aml_array_dbl_5d_copy(double *****dst, double *****src, int s1, int s2, int s3, int s4, int s5) {
   int i,j,k,l,m;
   for(i=0; i<s1; i++)
      for(j=0; j<s2; j++) 
         for(k=0; k<s3; k++)
            for(l=0; l<s4; l++)
               for(m=0; m<s5; m++)
                  dst[i][j][k][l][m] = src[i][j][k][l][m];
}

void aml_array_dbl_2d_variable_2nd_copy(double **dst, double **src, int s1, int *s2) {
   int i,j;
   for(i=0; i<s1; i++)
      for(j=0; j<s2[i]; j++)
         dst[i][j] = src[i][j];
}

// ARRAY DEALLOCATION
// pointless but necessary
void aml_array_int_1d_free(int *arr) {
   free(arr);
}

void aml_array_int_2d_free(int **arr, int s1) {
   int i;
   for(i=0; i<s1; i++)
      if(arr[i]) free(arr[i]);
   free(arr);
}

void aml_array_int_3d_free(int ***arr, int s1, int s2) {
   int i,j;
   for(i=0; i<s1; i++) {
      for(j=0; j<s2; j++)
         free(arr[i][j]);
      free(arr[i]);
   } free(arr);
}

void aml_array_int_4d_free(int ****arr, int s1, int s2, int s3) {
   int i,j,k;
   for(i=0; i<s1; i++) {
      for(j=0; j<s2; j++) {
         for(k=0; k<s3; k++)
            free(arr[i][j][k]);
         free(arr[i][j]);
      } free(arr[i]);
   } free(arr);
}

void aml_array_int_2d_variable_2nd_free(int **arr, int s1, int *s2) {
   int i;
   for(i=0; i<s1; i++)
      if(arr[i]) free(arr[i]);
   free(arr);
}

void aml_array_dbl_1d_free(double *arr) {
   free(arr);
}

void aml_array_dbl_2d_free(double **arr, int s1) {
   int i;
   for(i=0; i<s1; i++)
      if(arr[i]) free(arr[i]);
   free(arr);
}

void aml_array_dbl_3d_free(double ***arr, int s1, int s2) {
   int i,j;
   for(i=0; i<s1; i++) {
      for(j=0; j<s2; j++)
         free(arr[i][j]);
      free(arr[i]);
   } free(arr);
}

void aml_array_dbl_4d_free(double ****arr, int s1, int s2, int s3) {
   int i,j,k;
   for(i=0; i<s1; i++) {
      for(j=0; j<s2; j++) {
         for(k=0; k<s3; k++)
            free(arr[i][j][k]);
         free(arr[i][j]);
      } free(arr[i]);
   } free(arr);
}

void aml_array_dbl_2d_variable_2nd_free(double **arr, int s1, int *s2) {
   int i;
   for(i=0; i<s1; i++)
      if(arr[i]) free(arr[i]);
   free(arr);
}

void aml_array_dbl_3d_variable_2nd_free(double ***arr, int s1, int *s2) {
   int i,j;
   for(i=0; i<s1; i++) {
      for(j=0; j<s2[i]; j++)
         free(arr[i][j]);
      if(s2[i])
         free(arr[i]);
   } free(arr);
}


void aml_array_dbl_4d_variable_2nd_free(double ****arr, int s1, int *s2, int s3) {
   int i,j,k;
   for(i=0; i<s1; i++) {
      for(j=0; j<s2[i]; j++) {
         for(k=0; k<s3; k++)
            free(arr[i][j][k]);
         free(arr[i][j]);
      }
      free(arr[i]);
   } free(arr);
}

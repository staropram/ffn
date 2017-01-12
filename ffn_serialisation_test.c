#include <math.h>
#include <time.h>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>

#include <easy_rng.h>

//#include <aml/aml_dataset_wireless.h>
#include "ffn.h"
#include "rng.h"
#include "fio.h"

#undef DMALLOC
#ifdef DMALLOC
#include "dmalloc.h"
#endif

void print_dots() {
   printf("................................................................................\n");
}

int main(int argc, char **argv) {
   // local
   int i,j;
   FILE *f;
   aml_ffn *ffn1,*ffn2;

   // test housekeeping
   int test_count = 1;
   int pass_count = 0;

   // num layers
   int nl = 100;
   // layer cardinalities
   int lcc[] = {1 ,2,3,4,5,6,7,8,9,10,
                10,9,8,7,6,5,4,3,2,1 ,
                1 ,2,3,4,5,6,7,8,9,10,
                10,9,8,7,6,5,4,3,2,1 ,
                1 ,2,3,4,5,6,7,8,9,10,
                10,9,8,7,6,5,4,3,2,1 ,
                1 ,2,3,4,5,6,7,8,9,10,
                10,9,8,7,6,5,4,3,2,1 ,
                1 ,2,3,4,5,6,7,8,9,10,
                10,9,8,7,6,5,4,3,2,1};
                
   int *lc = &lcc[0];

   // weight init function
   char *wifn = "easy_ran_flat";
   double wif_pa = 0;
   double wif_pb = 1;
   double lrate = 0.1;
   double mrate = 0.01;
   double lwb = -1;
   double uwb = 1;
   int elwb = 0;
   int euwb = 0;
   int nafn = 3;
   char **afn = malloc(nafn*sizeof(char*));
   afn[0] = strdup("aml_ffn_linear");
   afn[1] = strdup("aml_ffn_sigmoid");
   afn[2] = strdup("aml_ffn_tanh");
   // activation function indices
   int **afi = malloc(nl*sizeof(int*));
   for(i=0; i<nl; i++) {
      afi[i] = malloc(lc[i]*sizeof(int));
      for(j=0; j<lc[i]; j++)
         afi[i][j] = j%3;
   }

   // init rng
   easy_rng *rng = aml_rng_init(-1);

   // create ffn
   ffn1 = aml_ffn_new(
      nl,     // number of layers
      lc,     // layer cardinality
      wifn,   // weight init function name
      rng,    // rng
      wif_pa, // weight init function parameter a
      wif_pb, // weight init function parameter b
      lrate,  // learning rate
      mrate,  // learning rate
      lwb,    // lower bound on weights
      uwb,    // upper bound on weights
      elwb,   // enforce lower weight bounds?
      euwb,   // enforce upper weight bounds?
      nafn,   // num diff activation function names
      afn,    // activation function names
      afi     // activation function indices
   );

   // save ffn
   f = fopen("ffn1.dat","w");
   aml_ffn_save(f,ffn1);
   fclose(f);
   
   // load it back in
   f = fopen("ffn1.dat","r");
   ffn2 = aml_ffn_load(f);
   fclose(f);
   
   // save it again
   f = fopen("ffn2.dat","w");
   aml_ffn_save(f,ffn2);
   fclose(f);

   // do a diff
   if(system("diff -q ffn1.dat ffn2.dat")) {
      printf("Test(1) : Comparison of ffn1.dat and ffn2.dat                           : FAILED\n");
      print_dots();
   } else {
      printf("Test(1)                                                                 : PASSED\n");
      print_dots();
      pass_count++;
   }

   if(test_count==pass_count) {
      printf("TESTING COMPLETE -- ALL TESTS PASSED                                    : PASSED\n");
      print_dots();
      system("rm ffn1.dat"); system("rm ffn2.dat");
   } else {
      printf("TESTING COMPLETE -- THERE WERE FAILURES : %d OUT OF %d TESTS FAILED     : FAILED\n",(test_count-pass_count),test_count);
      print_dots();
   }
   return 0;


   // clean up and quit
   aml_ffn_free(ffn1);
   aml_ffn_free(ffn2);
   for(i=0; i<nafn; i++)
      free(afn[i]);
   free(afn);
   return 0;
}

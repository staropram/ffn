# ffn
A simple feed-forward neural network library that supports per-unit activation function control

I wrote this probably 10 years ago but thought it might be useful to someone.

Here is an example of simple usage to solve the most trivial of non-linear problems: XOR


```c
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>

#include <easy_rng.h>

#include "fio.h"
#include "ffn.h"
#include "rng.h"

int main(int argc, char **argv) {

   // create a new ffn
   aml_ffn *aml_ffn_a = aml_ffn_new_simple(
      2, // inputs
      1, // hidden layers
      5, // units per hidden layer
      2, // num outputs
      0.0, // lower weight init bound
      1.0, // upper weight init bound
      "AML_FFN_LINEAR", // input layer activation fnction
      "AML_FFN_TANH", // hidden layer activation fnction
      "AML_FFN_LINEAR", // output layer activation fnction
      0L // PRNG seed
   );
   // a slightly faster learning rate than the default
   aml_ffn_a->lrate = 0.2;

   // print the ffn
   aml_ffn_printf(stdout,aml_ffn_a);

   // setup the XOR problem data
   int nte = 4; // 4 training examples
   double in[4][2]  = {
      {0.0, 0.0},
      {0.0, 1.0},
      {1.0, 0.0},
      {1.0, 1.0}
   };

   double out[4][1] = {
      {1.0},
      {0.0},
      {0.0},
      {1.0}
   };
      
   // feed-forward and backpropagate until error is low enough
   int cte = 0;
   int iteration = 0;
   int ll = aml_ffn_a->nl-1;
   double e =1;
   while(1) {
      // simulate
      aml_ffn_ff(aml_ffn_a,in[cte]);

      // print outputs
      printf("D: %lf, A: %lf\n",out[cte][0],aml_ffn_a->act[ll][0]);
      e +=
         (out[cte][0]-aml_ffn_a->act[ll][0])*
         (out[cte][0]-aml_ffn_a->act[ll][0]);

      // backprop
      aml_ffn_bp(aml_ffn_a,out[cte]);

      // increment training example
      cte++;
      cte %= nte;
      if(cte==0) {
         printf("-i %d --- error: %lf\n",iteration++,e);
         printf("--------------\n");
         if(e<0.0001) break;
         e = 0;
      }
   }

   aml_ffn_free(aml_ffn_a);

   return 0;
}
```

#include <math.h>
#include <stdio.h>
#define __USE_GNU
#include <dlfcn.h>
#include <stdlib.h>
#include <easy_rng.h>

#include "rng.h"
#include "ffn.h"
#include "array.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

//#define RTLD_DEFAULT RTLD_LAZY
//#define RTLD_DEFAULT RTLD_NOW

/* standard feed-forward neural-network */
/*
aml_ffn* aml_ffn_new_from_config(aml_ffnConfig *ffnc) {
   return aml_ffn_new(
      ffnc->nl,          // number of layers
      ffnc->lc,          // layer cardinality
      ffnc->wif,         // weight init function
      ffnc->rng,         // rng
      ffnc->wif_pa,      // weight init function parameter a
      ffnc->wif_pb,      // weight init function parameter b
      ffnc->lrate,       // learning rate
      ffnc->mrate,       // momentum rate
      ffnc->lwb,         // lower bound on weights
      ffnc->uwb,         // upper bound on weights
      ffnc->enforce_lwb, // enforce lower weight bounds?
      ffnc->enforce_uwb  // enforce upper weight bounds?
   );
}
*/

aml_ffn* aml_ffn_new_simple(
      int nin,
      int nhl,
      int nuhl,
      int nout,
      double w_lower,
      double w_upper,
      char *in_act_name,
      char *hid_act_name,
      char *out_act_name,
      long rng_seed
   ) {

   // local
   aml_ffn *ffn;
   int i,j,*lc,**afi;
   int in_act,hid_act,out_act;

   // init rng
   easy_rng *rng = aml_rng_init(rng_seed);

   // get ids of activation functions
   in_act  = aml_ffn_get_act_id_from_name(in_act_name);
   hid_act = aml_ffn_get_act_id_from_name(hid_act_name);
   out_act = aml_ffn_get_act_id_from_name(out_act_name);

   lc = malloc((2+nhl)*sizeof(int));
   // input cardinality
   lc[0] = nin;
   // hidden layer cardinality
   for(i=1; i<1+nhl; i++) lc[i] = nuhl;
   // output cardinality
   lc[1+nhl] = nout;

   // weight init function and parameters
   char *wifn = "easy_rng_uniform";

   // setup unit activation functions
   char **afn = malloc(3*sizeof(char*));
   afn[0] = strdup("aml_ffn_linear");
   afn[1] = strdup("aml_ffn_sigmoid");
   afn[2] = strdup("aml_ffn_tanh");
   afi = malloc((2+nhl)*sizeof(int*));
   afi[0] = malloc(nin*sizeof(int));
   for(i=0; i<nin; i++)
      afi[0][i] = in_act;
   for(i=0; i<nhl; i++) {
      afi[1+i] = malloc(nuhl*sizeof(int));
      for(j=0; j<nuhl; j++)
         afi[1+i][j] = hid_act;
   }
   afi[nhl+1] = malloc(nout*sizeof(int));
   for(i=0; i<nout; i++) {
      afi[nhl+1][i] = out_act;
   }

   // create ffn
   ffn = aml_ffn_new(
      nhl+2,     // number of layers
      lc,        // layer cardinality
      wifn,      // weight init function name
      rng,       // rng
      w_lower,   // weight init function parameter a
      w_upper,   // weight init function parameter b
      0.1,       // learning rate
      0,      // momentum rate
      0,      // lower bound on weights
      0,      // upper bound on weights
      0, // enforce lower weight bounds?
      0, // enforce upper weight bounds?
      3,        // num diff activation function names
      afn,      // activation function names
      afi        // indices of af for each unit
   );

   // clean up
   for(i=0; i<3; i++)
      free(afn[i]);
   free(afn);
   free(lc);

   // return
   return ffn;
}

int aml_ffn_get_act_id_from_name(char *act_name) {
   if(!strcmp(act_name,"AML_FFN_LINEAR")) 
      return AML_FFN_LINEAR;
   else if(!strcmp(act_name,"AML_FFN_SIGMOID"))
      return AML_FFN_SIGMOID;
   else if(!strcmp(act_name,"AML_FFN_TANH"))
      return AML_FFN_TANH;
   else
      return AML_FFN_LINEAR;
}

char* aml_ffn_get_prime_function_name(char *name) {
   int new_len = strlen(name)+1+6;
   char *prime_name = strdup(name);
   prime_name = realloc(prime_name,strlen(prime_name)+1+6);
   prime_name[new_len-1]=0x0; prime_name[new_len-2]='e';
   prime_name[new_len-3]='m'; prime_name[new_len-4]='i';
   prime_name[new_len-5]='r'; prime_name[new_len-6]='p';
   prime_name[new_len-7]='_';
   return prime_name;
}

// load activation function names, num is a pointer which stores the result
char** aml_ffn_load_af_names(FILE *f, int *num) {
   int afn_size = 0;
   char **afn = NULL;
   do {
      afn_size++;
      afn = realloc(afn,afn_size*sizeof(char*));
      afn[afn_size-1] = malloc(100*sizeof(char));
   } while(fscanf(f,"%s",afn[afn_size-1])!=EOF);
   afn_size--;
   free(afn[afn_size]);
   afn = realloc(afn,afn_size*sizeof(char*));

   *num = afn_size;
   return afn;
}

void aml_ffn_dlfunc_af(aml_ffn *ffn) {
   int i;
   char *prime_name;
   // allocate space for functions
   ffn->af  = malloc(ffn->nafn*sizeof(aml_ffn_af));
   ffn->afp = malloc(ffn->nafn*sizeof(aml_ffn_afp));
   // dlfunc the activation functions and the derivative functions
   for(i=0; i<ffn->nafn; i++) {
      ffn->af[i] =
         (aml_ffn_af)dlfunc(RTLD_DEFAULT,ffn->afn[i]);
      if(!ffn->af[i]) {
         fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,dlerror()); exit(1);
      }
      // construct the "_prime" version of the current activation function
      prime_name = aml_ffn_get_prime_function_name(ffn->afn[i]);
      ffn->afp[i] =
         (aml_ffn_afp)dlfunc(RTLD_DEFAULT,prime_name);
      if(!ffn->afp[i]) {
         fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,dlerror()); exit(1);
      }
      free(prime_name);
   }
}

aml_ffn* aml_ffn_new(
   int nl,          // number of layers
   int *lc,         // layer cardinality
   char *wifn,      // weight init function name
   easy_rng *rng,    // rng
   double wif_pa,   // weight init function parameter a
   double wif_pb,   // weight init function parameter b
   double lrate,    // learning rate
   double mrate,    // momentum rate
   double lwb,      // lower bound on weights
   double uwb,      // upper bound on weights
   int enforce_lwb, // enforce lower weight bounds?
   int enforce_uwb, // enforce upper weight bounds?
   int nafn,        // num diff activation function names
   char **afn,      // activation function names
   int **afi        // indices of af for each unit
   ) {

   // local
   int i,l,u;

   /*
   printf("nl: %d\n",nl);
   for(i=0; i<nl; i++) {
      printf("lc[%d]: %d\n",i,lc[i]);
   }
   */

   // malloc
   aml_ffn *ffn = malloc(sizeof(aml_ffn));
   ffn->lrate         = lrate;
   ffn->mrate         = mrate;
   ffn->nl            = nl;
   // each layer has a BIAS unit except the output layer which has no bias
   // unit, it is allocated anyway but subsequently ignored, so one needs to
   // be added to each layer cardinality
   ffn->lc = malloc(nl*sizeof(int));
   for(l=0; l<nl; l++)
      ffn->lc[l] = lc[l] + 1;

   ffn->af  = NULL;
   ffn->afp = NULL;

   // load activation function names
   ffn->nafn          = nafn;
   ffn->afn           = malloc(nafn*sizeof(char*));
   for(i=0; i<nafn; i++) ffn->afn[i] = strdup(afn[i]);

   // load the activation functions
   (void)aml_ffn_dlfunc_af(ffn);

   // load the weight init function
   ffn->wifn = strdup(wifn);
   ffn->wif = (aml_ffn_wif) dlfunc(RTLD_DEFAULT,ffn->wifn);
   if(!ffn->wif) {
      fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,dlerror()); exit(1);
   }

   // allocate space for ffn deltas, used during back-propagation
   ffn->deltas = aml_array_dbl_2d_variable_2nd_new(ffn->nl,ffn->lc,0);
   // allocate space for the unit-to-activation-function map
   ffn->afi    = aml_array_int_2d_variable_2nd_new(ffn->nl,ffn->lc,-1);
   // allocate space for unit activations
   // the activations of bias units are always 1, the other units will be
   // reset to 0 activation at the start of any feed-forward-propagation
   ffn->act    = aml_array_dbl_2d_variable_2nd_new(ffn->nl,ffn->lc,1);
   // allocate space for network activations (summed activity before function)
   ffn->net    = aml_array_dbl_2d_variable_2nd_new(ffn->nl,ffn->lc,0);
   // allocate space for activation function indices

   // associate activation functions with units
   if(afi) {
      for(l=0; l<nl; l++)
         for(u=0; u<ffn->lc[l]; u++)
            ffn->afi[l][u] = afi[l][u];
   }

   // malloc weights and last_weight_changes
   ffn->weights = aml_ffn_weights_array_malloc(ffn->nl,ffn->lc);
   ffn->last_weight_changes = aml_ffn_weights_array_malloc(ffn->nl,ffn->lc);

   ffn->lwb = lwb;
   ffn->uwb = uwb;
   ffn->wif_pa = wif_pa;
   ffn->wif_pb = wif_pb;
   ffn->enforce_lwb = enforce_lwb;
   ffn->enforce_uwb = enforce_uwb;

   // initialise the weights according to the activation function
   aml_ffn_init_weights(ffn,rng);
   // bound the weights accordingly
   aml_ffn_bound_weights(ffn);

   return ffn;
}

void aml_ffn_weights_copy(double ***dst, double ***src, int nl, int *lc) {
   int l,u,nu;
   for(l=0; l<nl-1; l++)
      for(u=0; u<lc[l]; u++)
         for(nu=0; nu<lc[l+1]; nu++)
            dst[l][u][nu] = src[l][u][nu];
}

double*** aml_ffn_weights_array_malloc(int nl, int *lc) {
   int l,u,nu;
   double ***weights = malloc(nl*sizeof(double**));
   for(l=0; l<nl-1; l++) {
      weights[l] = malloc(lc[l]*sizeof(double*));
      for(u=0; u<lc[l]; u++) {
         weights[l][u] = malloc(lc[l+1]*sizeof(double));
         for(nu=0; nu<lc[l+1]; nu++) {
            weights[l][u][nu] = 0;
         }
      }
   }
   return weights;
}


void aml_ffn_save(FILE *f, aml_ffn *ffn) {
   int i;
   fprintf(f,"nl: %d\n",ffn->nl);
   aml_array_int_1d_save(f,ffn->lc,ffn->nl);
   fprintf(f,"\nnafn: %d\n",ffn->nafn);
   for(i=0; i<ffn->nafn; i++) fprintf(f,"%s\n",ffn->afn[i]);
   // print wif
   fprintf(f,"wif_pa: %lf\n",ffn->wif_pa);
   fprintf(f,"wif_pb: %lf\n",ffn->wif_pb);
   fprintf(f,"wif_name: %s\n",ffn->wifn);
   // activation function indices for net
   fprintf(f,"activation_function_indices:\n");
   aml_array_int_2d_variable_2nd_save(f,ffn->afi,ffn->nl,ffn->lc);
   fprintf(f,"\n");
   // unit activations
   fprintf(f,"unit_activations:\n");
   aml_array_dbl_2d_variable_2nd_save(f,ffn->act,ffn->nl,ffn->lc);
   fprintf(f,"\n");
   // net
   fprintf(f,"net_activations:\n");
   aml_array_dbl_2d_variable_2nd_save(f,ffn->net,ffn->nl,ffn->lc);
   fprintf(f,"\n");
   // weights
   fprintf(f,"weights:\n");
   aml_ffn_weights_array_save(f,ffn->weights,ffn->nl,ffn->lc);
   fprintf(f,"\n");
   // IGNORE last_weight_changes
   // IGNORE deltas
   // learning rate
   fprintf(f,"learning_rate: %lf\n",ffn->lrate);
   // momentum rate
   fprintf(f,"momentum_rate: %lf\n",ffn->mrate);
   // lower weight bound
   fprintf(f,"lower_weight_bound: %lf\n",ffn->lwb);
   // upper weight bound
   fprintf(f,"upper_weight_bound: %lf\n",ffn->uwb);
   // enforce lower weight bound
   fprintf(f,"enforce_lower_weight_bound: %d\n",ffn->enforce_lwb);
   // enforce upper weight bound
   fprintf(f,"enforce_upper_weight_bound: %d\n",ffn->enforce_uwb);
}

aml_ffn* aml_ffn_load(FILE *f) {
   int i;
   char *buf = malloc(300*sizeof(char));

   aml_ffn *ffn = malloc(sizeof(aml_ffn));

   fscanf(f,"%*s %d\n",&ffn->nl);
   ffn->lc = aml_array_int_1d_load(f,ffn->nl);
   fscanf(f,"%*s %d\n",&ffn->nafn);
   // activation function names, and prime names
   ffn->afn = malloc(ffn->nafn*sizeof(char*));
   for(i=0; i<ffn->nafn; i++) {
      fscanf(f,"%s\n",buf);
      ffn->afn[i] = strdup(buf);
   }
   // load the activation functions
   (void)aml_ffn_dlfunc_af(ffn);
   fscanf(f,"%*s %lf\n",&ffn->wif_pa);
   fscanf(f,"%*s %lf\n",&ffn->wif_pb);
   // load the weight init function
   fscanf(f,"%*s %s\n",buf);
   ffn->wifn = strdup(buf);
   ffn->wif = (aml_ffn_wif) dlfunc(RTLD_DEFAULT,ffn->wifn);
   if(!ffn->wif) {
      fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,dlerror()); exit(1);
   }
 
   // activation function indices for net
   fscanf(f,"%*s");
   ffn->afi = aml_array_int_2d_variable_2nd_load(f,ffn->nl,ffn->lc);
   // unit activations
   fscanf(f,"%*s");
   ffn->act = aml_array_dbl_2d_variable_2nd_load(f,ffn->nl,ffn->lc);
   // net
   fscanf(f,"%*s");
   ffn->net = aml_array_dbl_2d_variable_2nd_load(f,ffn->nl,ffn->lc);
   // load weight values
   fscanf(f,"%*s");
   ffn->weights = aml_ffn_weights_array_load(f,ffn->nl,ffn->lc);
   // malloc last_weight_changes
   ffn->last_weight_changes = aml_ffn_weights_array_malloc(ffn->nl,ffn->lc);
   // init space for deltas
   ffn->deltas = aml_array_dbl_2d_variable_2nd_new(ffn->nl,ffn->lc,0);
   // learning rate
   fscanf(f,"%*s %lf\n",&ffn->lrate);
   // momentum rate
   fscanf(f,"%*s %lf\n",&ffn->mrate);
   // lower weight bound
   fscanf(f,"%*s %lf\n",&ffn->lwb);
   // upper weight bound
   fscanf(f,"%*s %lf\n",&ffn->uwb);
   // enforce lower weight bound
   fscanf(f,"%*s %d\n",&ffn->enforce_lwb);
   // enforce upper weight bound
   fscanf(f,"%*s %d\n",&ffn->enforce_uwb);
   return ffn;
}

void aml_ffn_weights_array_save(FILE *f, double ***weights, int nl, int *lc) {
   int l,u,nu;
   for(l=0; l<nl-1; l++)
      for(u=0; u<lc[l]; u++)
         for(nu=0; nu<lc[l+1]; nu++)
            fprintf(f,"%lf ",weights[l][u][nu]);
}

double*** aml_ffn_weights_array_load(FILE *f, int nl, int *lc) {
   double ***weights = aml_ffn_weights_array_malloc(nl,lc);
   int l,u,nu;
   for(l=0; l<nl-1; l++)
      for(u=0; u<lc[l]; u++)
         for(nu=0; nu<lc[l+1]; nu++)
            fscanf(f,"%lf ",&weights[l][u][nu]);
   return weights;
}

void aml_ffn_weights_array_load_into(FILE *f, double ***weights, int nl, int *lc) {
   int l,u,nu;
   for(l=0; l<nl-1; l++)
      for(u=0; u<lc[l]; u++)
         for(nu=0; nu<lc[l+1]; nu++)
            fscanf(f,"%lf ",&weights[l][u][nu]);
}

void aml_ffn_weights_array_free(double ***weights, int nl, int *lc) {
   int l,u;
   for(l=0; l<nl-1; l++) {
      for(u=0; u<lc[l]; u++)
         free(weights[l][u]);
      free(weights[l]);
   }
   free(weights);
}

void aml_ffn_init_weights(
   aml_ffn *ffn,  
   easy_rng *rng    // rng
   ) {

   int l,u,nu;
   for(l=0; l<ffn->nl-1; l++)
      for(u=0; u<ffn->lc[l]; u++)
         for(nu=0; nu<ffn->lc[l+1]; nu++)
            ffn->weights[l][u][nu] = ffn->wif(rng,ffn->wif_pa,ffn->wif_pb);
}

void aml_ffn_load_afunctions_and_derivatives(aml_ffn *ffn, FILE *f) {
   ffn->nafn = 0;

   char c;
   char *buffer = malloc(250);
   int pos;
   c = getc(f);
   while(c==0xa||c==' '||c=='\t') c = getc(f);
   while(c!=EOF) {
      // extract function name
      pos = 0;
      while(c!=0xa&&c!=' '&&c!='\t'&&c!=EOF) {
         buffer[pos++] = c;
         c = getc(f);
      } buffer[pos] = 0x0;
      while(c==0xa||c==' '||c=='\t') c = getc(f);

      // if function name extracted
      if(pos) {
         // expand af and afp
         ffn->nafn++;
         ffn->af = realloc(
            ffn->af,
            ffn->nafn*sizeof(aml_ffn_af)
         );
         ffn->afp = realloc(
            ffn->afp,
            ffn->nafn*sizeof(aml_ffn_afp)
         );
         // load af
         ffn->af[ffn->nafn-1] =
            (aml_ffn_af) dlfunc(RTLD_DEFAULT,buffer);
         if(!ffn->af[ffn->nafn-1]) {
            fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,dlerror()); exit(1);
         }
         // load afp
         buffer[pos++] = '_'; buffer[pos++] = 'p'; buffer[pos++] = 'r';
         buffer[pos++] = 'i'; buffer[pos++] = 'm'; buffer[pos++] = 'e';
         buffer[pos] = 0x0;
         ffn->afp[ffn->nafn-1] = (aml_ffn_afp) dlfunc(RTLD_DEFAULT,buffer);
         if(!ffn->afp[ffn->nafn-1]) {
            fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,dlerror()); exit(1);
         }
      }
   }
}

// simulation, an assumption is that the act are already established
void aml_ffn_ff(aml_ffn *ffn, double *net) {
   int l=0,u,pu;

   // set activation of input units, leave bias alone
   for(u=0; u<ffn->lc[0]-1; u++) {
      if(ffn->afi[0][u]!=-1) {
         ffn->act[0][u] = (ffn->af[ffn->afi[0][u]])(net[u]);
      } else {
         ffn->act[0][u] = net[u];
      }
   }

   for(l=1; l<ffn->nl; l++) {
      // iterate over current units, ignoring the bias unit
      for(u=0; u<ffn->lc[l]-1; u++) {
         ffn->net[l][u] = 0;
         // iterate over preceeding units, including the bias unit
         for(pu=0; pu<ffn->lc[l-1]; pu++) {
            ffn->net[l][u] += ffn->act[l-1][pu]*ffn->weights[l-1][pu][u];
         }
         // set activation of unit
         ffn->act[l][u] = (ffn->af[ffn->afi[l][u]])(ffn->net[l][u]);
         /*
         if(l==ffn->nl-1) {
         printf("ffn->net[%d][%d]: %lf\n",l,u,ffn->net[l][u]);
         printf("ffn->act[%d][%d]: %lf\n",l,u,ffn->act[l][u]);
         printf("ffn->afi[%d][%d]: %d\n",l,u,ffn->afi[l][u]);
         }
         */
      }
   }
}

// calculates deltas
void aml_ffn_calc_deltas(aml_ffn *ffn, double *desired) {
   int l,u,nu;
   int ol = ffn->nl-1;
   double derivative_of_activation = 0;
   double derivative_of_out_with_respect_to_error = 0;
   double delta_sum = 0;

   // calculate output deltas, ignore bias
   for(u=0; u<ffn->lc[ol]-1; u++) {
      derivative_of_out_with_respect_to_error =
         -(desired[u] - ffn->act[ol][u]);
      derivative_of_activation =
         (ffn->afp[ffn->afi[ol][u]])
            (ffn->net[ol][u],ffn->act[ol][u]);
      ffn->deltas[ol][u] = derivative_of_out_with_respect_to_error *
         derivative_of_activation;
   }

   // calculate hidden deltas
   for(l=ffn->nl-2; l>0; l--) {
      // ignore bias unit
      for(u=0; u<ffn->lc[l]-1; u++) {
         derivative_of_activation =
            (ffn->afp[ffn->afi[l][u]])
               (ffn->net[l][u],ffn->act[l][u]);
         delta_sum = 0;
         // ignore bias unit
         for(nu=0; nu<ffn->lc[l+1]-1; nu++)
            delta_sum += ffn->deltas[l+1][nu]*ffn->weights[l][u][nu];
         ffn->deltas[l][u] = derivative_of_activation * delta_sum;
      }
   }
}

void aml_ffn_bp(aml_ffn *ffn, double *desired) {
   int l,u,nu;
   double weight_change = 0;

   // calculate deltas
   aml_ffn_calc_deltas(ffn,desired);

   // update weights
   if(ffn->mrate==0) {
      for(l=ffn->nl-2; l>=0; l--) {
         for(u=0; u<ffn->lc[l]; u++) {
            // ignore weights to bias unit since they are not used
            for(nu=0; nu<ffn->lc[l+1]-1; nu++) {
               weight_change =
                  -ffn->lrate * ffn->deltas[l+1][nu] * ffn->act[l][u];
               ffn->weights[l][u][nu] += weight_change;
            }
         }
      }
   } else {
      for(l=ffn->nl-2; l>=0; l--) {
         for(u=0; u<ffn->lc[l]; u++) {
            // ignore weights to bias unit since they are not used
            for(nu=0; nu<ffn->lc[l+1]-1; nu++) {
               weight_change =
                  -ffn->lrate * ffn->deltas[l+1][nu] * ffn->act[l][u] +
                  ffn->mrate * ffn->last_weight_changes[l][u][nu];
               ffn->last_weight_changes[l][u][nu] = weight_change;
               ffn->weights[l][u][nu] += weight_change;
            }
         }
      }
   }
   aml_ffn_bound_weights(ffn);
}

void aml_ffn_bound_weights(aml_ffn *ffn) {
   int l,u,nu;
   if(ffn->enforce_lwb) {
      // ignore weights to bias unit since they are not used
      for(l=ffn->nl-2; l>=0; l--)
         for(u=0; u<ffn->lc[l]; u++)
            for(nu=0; nu<ffn->lc[l+1]-1; nu++)
               if(ffn->weights[l][u][nu]<ffn->lwb)
                  ffn->weights[l][u][nu]=ffn->lwb;
   }

   if(ffn->enforce_uwb) {
      // ignore weights to bias unit since they are not used
      for(l=ffn->nl-2; l>=0; l--)
         for(u=0; u<ffn->lc[l]; u++)
            for(nu=0; nu<ffn->lc[l+1]-1; nu++)
               if(ffn->weights[l][u][nu]>ffn->uwb)
                  ffn->weights[l][u][nu]=ffn->uwb;
   }
}

double aml_ffn_ff_and_bp_batch(aml_ffn *ffn, double **in, double **out, int length) {
   int p,l,u,nu;
   double cum_error = 0;
   double weight_change;
   // make storage for cumulative weight change
   double ***cum_weight_change = malloc(ffn->nl*sizeof(double**));
   for(l=0; l<ffn->nl-1; l++) {
      cum_weight_change[l]     = malloc(ffn->lc[l]*sizeof(double*));
      for(u=0; u<ffn->lc[l]; u++) {
         cum_weight_change[l][u] = malloc(ffn->lc[l+1]*sizeof(double));
         for(nu=0; nu<ffn->lc[l+1]; nu++) {
            cum_weight_change[l][u][nu] = 0;
         }
      }
   }

   // process each input pattern, compute deltas against desired outputs
   // and add up cumulative weight change
   for(p=0; p<length; p++) {
      // process
      aml_ffn_ff(ffn,in[p]);
      // accrue error, ignore bias
      for(u=0; u<ffn->lc[ffn->nl-1]-1; u++) {
         cum_error += pow((out[p][u]-ffn->act[ffn->nl-1][u]),2);
      }
      printf("D: %lf, A: %lf\n",out[p][0],ffn->act[ffn->nl-1][0]);
      // compute deltas
      aml_ffn_calc_deltas(ffn,out[p]);

      // accrue weight change
      for(l=ffn->nl-2; l>=0; l--) {
         for(u=0; u<ffn->lc[l]; u++) {
            // ignore weights to bias unit since they are not used
            for(nu=0; nu<ffn->lc[l+1]-1; nu++) {
               cum_weight_change[l][u][nu] +=
                  ffn->deltas[l+1][nu] * ffn->act[l][u];
            }
         }
      }
   }

   // update weights from cum_weight_change
   for(l=ffn->nl-2; l>=0; l--) {
      for(u=0; u<ffn->lc[l]; u++) {
         // ignore weights to bias unit since they are not used
         for(nu=0; nu<ffn->lc[l+1]-1; nu++) {
            weight_change = -ffn->lrate*cum_weight_change[l][u][nu]+
               ffn->mrate * ffn->last_weight_changes[l][u][nu];
            ffn->last_weight_changes[l][u][nu] = weight_change;
            ffn->weights[l][u][nu] += weight_change;
         }
      }
   }
   
   // bound weights if necessary
   aml_ffn_bound_weights(ffn);

   // free cum_weight_change
   for(l=0; l<ffn->nl-1; l++) {
      for(u=0; u<ffn->lc[u]; u++) {
         free(cum_weight_change[l][u]);
      } free(cum_weight_change[l]);
   } free(cum_weight_change);

   // return cumulative error
   return cum_error;
}

// diagnostic
void aml_ffn_printf(FILE *f, aml_ffn *ffn) {
   int a,l,u,nu;
   fprintf(f,"nl: %d\n",ffn->nl);
   fprintf(f,"lc:\n");
   for(l=0; l<ffn->nl; l++) fprintf(f,"%d ",ffn->lc[l]);
   fprintf(f,"\nnafn: %d\n",ffn->nafn);
   // act_f
   fprintf(f,"act_f:\n");
   for(a=0; a<ffn->nafn; a++)
      fprintf(f,"0x%x ",(int)ffn->af[a]);
   // act_fp
   fprintf(f,"\nact_fp:\n");
   for(a=0; a<ffn->nafn; a++)
      fprintf(f,"0x%x ",(int)ffn->afp[a]);
   // act_fi
   fprintf(f,"\nact_fi:\n");
   for(l=0; l<ffn->nl; l++) {
      for(u=0; u<ffn->lc[l]; u++)
         fprintf(f,"%d ",ffn->afi[l][u]);
      fprintf(f,"\n");
   }
   // act
   fprintf(f,"act:\n");
   for(l=0; l<ffn->nl; l++) {
      for(u=0; u<ffn->lc[l]; u++)
         fprintf(f,"%lf ",ffn->act[l][u]);
      fprintf(f,"\n");
   }
   // net 
   fprintf(f,"net:\n");
   for(l=0; l<ffn->nl; l++) {
      for(u=0; u<ffn->lc[l]; u++)
         fprintf(f,"%lf ",ffn->net[l][u]);
      fprintf(f,"\n");
   }
   // weights
   fprintf(f,"weights:\n");
   for(l=0; l<ffn->nl-1; l++) {
      for(u=0; u<ffn->lc[l]; u++)
         for(nu=0; nu<ffn->lc[l+1]; nu++)
            fprintf(f,"%lf ",ffn->weights[l][u][nu]);
      fprintf(f,"\n");
   }
   // last weight changes 
   fprintf(f,"last weight changes:\n");
   for(l=0; l<ffn->nl-1; l++) {
      for(u=0; u<ffn->lc[l]; u++)
         for(nu=0; nu<ffn->lc[l+1]; nu++)
            fprintf(f,"%lf ",ffn->last_weight_changes[l][u][nu]);
      fprintf(f,"\n");
   }
   // deltas
   fprintf(f,"deltas:\n");
   for(l=0; l<ffn->nl; l++) {
      for(u=0; u<ffn->lc[l]; u++)
         fprintf(f,"%lf ",ffn->deltas[l][u]);
      fprintf(f,"\n");
   }
   // lrate
   fprintf(f,"lrate: %lf\n",ffn->lrate);
   // mrate
   fprintf(f,"mrate: %lf\n",ffn->mrate);
   // lwb
   fprintf(f,"lwb: %lf\n",ffn->lwb);
   // uwb
   fprintf(f,"uwb: %lf\n",ffn->uwb);
   // enforce_lwb
   fprintf(f,"enforce_lwb: %d\n",ffn->enforce_lwb);
   // enforce_lwb
   fprintf(f,"enforce_uwb: %d\n",ffn->enforce_uwb);
}

void aml_ffn_activation_profile_load_into(aml_ffn *ffn, FILE *f) {
   int l,u;
   for(l=0; l<ffn->nl; l++)
      for(u=0; u<ffn->lc[l]-1; u++)
         fscanf(f,"%d",&ffn->afi[l][u]);
}

// activation function profile, nl and lc are the modified versions
int** aml_ffn_activation_profile_load(FILE *f, int nl, int *lc) {
   int l,u;
   int **afi = malloc(nl*sizeof(int*));
   for(l=0; l<nl; l++) {
      afi[l] = malloc(lc[l]*sizeof(int));
      // do not load in activations for bias unit
      for(u=0; u<lc[l]-1; u++)
         fscanf(f,"%d",&afi[l][u]);
   }
   return afi;
}

// free an aml_ffn
void aml_ffn_free(aml_ffn *ffn) {
   int i;
   free(ffn->af);
   free(ffn->afp);
   for(i=0; i<ffn->nafn; i++)
      free(ffn->afn[i]);
   free(ffn->afn);
   aml_array_int_2d_variable_2nd_free(ffn->afi,ffn->nl,ffn->lc);
   aml_array_dbl_2d_variable_2nd_free(ffn->act,ffn->nl,ffn->lc); // act
   aml_array_dbl_2d_variable_2nd_free(ffn->net,ffn->nl,ffn->lc); // net
   aml_ffn_weights_array_free(ffn->weights,ffn->nl,ffn->lc);  // weights
   // last weight changes
   aml_ffn_weights_array_free(ffn->last_weight_changes,ffn->nl,ffn->lc);
   // deltas
   aml_array_dbl_2d_variable_2nd_free(ffn->deltas,ffn->nl,ffn->lc); // deltas 

   // free layer cardinality (this was used by other free functions so was left)
   free(ffn->lc);

   free(ffn);
}

// activation functions and their derivatives
double aml_ffn_sigmoid(double net) { return 1.0/(1.0 + exp(-net)); }
double aml_ffn_sigmoid_prime(double net, double a) { return a*(1-a); }
double aml_ffn_linear(double net)  { return net; }
double aml_ffn_linear_prime(double net, double a) { return 1; }
double aml_ffn_tanh(double net) { return tanh(net); }
double aml_ffn_tanh_prime(double net, double a) { return 1-(a*a); }
double aml_ffn_tanh10(double net) { return 10*tanh(net); }
double aml_ffn_tanh10_prime(double net, double a) { return 10*(1-pow(tanh(net),2)); }

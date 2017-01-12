#ifndef AML_FFN_H
#define AML_FFN_H
#define _GNU_SOURCE

#include <stdlib.h>
#include <easy_rng.h>

#include "fio.h"

#define dlfunc dlsym

// typedefs for activation function and derivative of (af prime)
typedef double (*aml_ffn_af)(double d);
typedef double (*aml_ffn_afp)(double net, double activation);

// weight init function
typedef double (*aml_ffn_wif)(
   const easy_rng *rng,const double pa,const double pb);

// builtin activation function types
enum {
   AML_FFN_LINEAR,
   AML_FFN_SIGMOID,
   AML_FFN_TANH
};

// returns the function type associated with the name
int aml_ffn_get_act_id_from_name(char *act_name);

// aml_ffn struct
typedef struct {
   int nl;
   int *lc;

   // maintain a list of activation functions and their derivatives
   int nafn; // num activation functions
   aml_ffn_af *af;   // activation functions
   aml_ffn_afp *afp; // their derivatives - activation function primes

   char *wifn;
   aml_ffn_wif wif;
   double wif_pa;
   double wif_pb;

   // need to store the actual names of the functions too, for retrieval
   char **afn; // activation function names

   // activation function for a particular node is an index into the list
   int **afi; // activation function indices

   double **act; // unit activations
   double **net; // cumulative input (prior to activation function)
   double ***weights;
   double ***last_weight_changes;
   double **deltas;

   double lrate;
   double mrate;

   double lwb; // lower weight bound
   double uwb; // upper weight bound
   int enforce_lwb; // enforce lower weight bound
   int enforce_uwb; // enforce upper weight bound
} aml_ffn;

// config struct
typedef struct {
   int nl;          // number of layers
   int *lc;         // layer cardinality
   aml_ffn_wif wif; // weight init function
   easy_rng *rng;    // rng
   double wif_pa;   // weight init function parameter a
   double wif_pb;   // weight init function parameter b
   double lrate;    // learning rate
   double mrate;    // momentum rate
   double lwb;      // lower bound on weights
   double uwb;      // upper bound on weights
   int enforce_lwb; // enforce lower weight bounds?
   int enforce_uwb; // enforce upper weight bounds?
} aml_ffn_config;

// creation and configuration routines
char* aml_ffn_get_prime_function_name(char *name);

aml_ffn* aml_ffn_new(
   int nl,          // number of layers
   int *lc,         // layer cardinality
   char *wifn,      // weight init function
   easy_rng *rng,    // rng
   double wif_pa,   // weight init function parameter a
   double wif_pb,   // weight init function parameter b
   double lrate,    // learning rate
   double mrate,    // learning rate
   double lwb,      // lower bound on weights
   double uwb,      // upper bound on weights
   int enforce_lwb, // enforce lower weight bounds?
   int enforce_uwb, // enforce upper weight bounds?
   int nafn,        // num diff activation function names
   char **afn,      // activation function names
   int **uaf        // unit activation functions (settings)
);

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
);

void aml_ffn_dlfunc_af(aml_ffn *ffn);
char** aml_ffn_load_af_names(FILE *f, int *num);

aml_ffn* aml_ffn_new_from_config(aml_ffn_config *ffnc);
char** aml_ffn_load_af_names(FILE *f, int *num);

double*** aml_ffn_weights_array_malloc(int nl, int *lc);
void aml_ffn_weights_array_save(FILE *f, double ***weights, int nl, int *lc);
double*** aml_ffn_weights_array_load(FILE *f, int nl, int *lc);
void aml_ffn_weights_array_free(double ***weights, int nl, int *lc);
void aml_ffn_weights_array_load_into(FILE *f, double ***weights, int nl, int *lc);
void aml_ffn_save(FILE* f, aml_ffn *ffn);
aml_ffn* aml_ffn_load(FILE* f);

void aml_ffn_init_weights(
   aml_ffn *ffn,  
   easy_rng *rng  // rng
);

void aml_ffn_load_activation_functions_and_derivatives(aml_ffn *ffn, FILE *f);
aml_ffn* aml_ffn_clone(aml_ffn *ffn);
void aml_ffn_free(aml_ffn *ffn);
void aml_ffn_weights_copy(double ***dst, double ***src, int nl, int *lc);

// simulation routines
void aml_ffn_ff(aml_ffn *ffn, double *net);
void aml_ffn_bp(aml_ffn *ffn, double *desired);
double ffn_ff_and_bp_batch(aml_ffn *ffn, double **in, double **out, int length);
void aml_ffn_calc_deltas(aml_ffn *ffn, double *desired);
void aml_ffn_bound_weights(aml_ffn *ffn);

// diagnostic
void aml_ffn_printf(FILE *f, aml_ffn *ffn);

// activation function profile
void aml_ffn_activation_profile_load_into(aml_ffn *ffn, FILE *f);
int** aml_ffn_activation_profile_load(FILE *f, int nl, int *lc);

// activation functions and their derivatives
double aml_ffn_sigmoid(double net);
double aml_ffn_sigmoid_prime(double net, double activation);
double aml_ffn_linear(double net);
double aml_ffn_linear_prime(double net, double activation);
double aml_ffn_tanh(double net);
double aml_ffn_tanh_prime(double net, double a);
double aml_ffn_tanh10(double net);
double aml_ffn_tanh10_prime(double net, double a);
double aml_ffn_tanh100(double net);
double aml_ffn_tanh100_prime(double net, double a);

#endif

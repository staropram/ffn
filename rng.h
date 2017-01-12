#ifndef AML_RNG_H
#define AML_RNG_H
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <easy_rng.h>

easy_rng* aml_rng_init(long rng_seed);
long aml_rng_get_random_seed();
#endif

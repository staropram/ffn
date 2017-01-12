#include <easy_rng.h>
#include "rng.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

easy_rng* aml_rng_init(long rng_seed) {
   easy_rng *rng = easy_rng_alloc(easy_rng_default);
   if(rng_seed!=-1) {
      easy_rng_set(rng,rng_seed);
   //   printf("setting rng to %ld\n",rng_seed);
   } else {
      struct timespec *t = (struct timespec*)malloc(sizeof(struct timespec));
      clock_gettime(CLOCK_REALTIME,t);
      easy_rng_set(rng,t->tv_nsec/(long)getpid());
      free(t);
   }
   return rng;
}

long aml_rng_get_random_seed() {
   long seed;
   struct timespec *t = (struct timespec*)malloc(sizeof(struct timespec));
   clock_gettime(CLOCK_REALTIME,t);
   seed = t->tv_nsec/(long)getpid();
   free(t);
   return seed;
}

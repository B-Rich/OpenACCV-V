#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    real_t * a = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));
    real_t * c_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * d = (real_t *)malloc(10 * n * sizeof(real_t));
    
    for (int x = 0; x < 10*n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        d[x] = 0.0;
    }
    for (int x = 0; x < n; ++x){
        c[x] = rand() / (real_t)(RAND_MAX / 10);
        c_copy[x] = c[x];
    }
    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copy(d[0:10*n])
    {
        #pragma acc parallel firstprivate(c[0:n])
        {
            #pragma acc loop gang
            for (int x = 0; x < 10; ++x){
                #pragma acc loop worker
                for (int y = 0; y < n; ++y){
                    d[x * n + y] = a[x * n + y] + b[x * n + y] + c[y];
                }
            }
        }
    }

    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            if (fabs(d[x * n + y] - (a[x * n + y] + b[x * n + y] + c_copy[y])) > PRECISION){
                err += 1;
            }
        }
    }
    
    for (int x = 0; x < 10*n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        d[x] = 0.0;
    }
    for (int x = 0; x < n; ++x){
        c[x] = 0.0;
    }
 
    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copy(d[0:10*n])
    {
        #pragma acc parallel firstprivate(c[0:n])
        {
            #pragma acc loop gang independent
            for (int x = 0; x < 10; ++x){
                #pragma acc loop worker independent
                for (int y = 0; y < n; ++y){
                    c[y] = a[x * n + y] - b[x * n + y];
                }
                #pragma acc loop worker independent
                for (int y = 0; y < n; ++y){
                    d[x * n + y] = a[x * n + y] + b[x * n + y] + c[y];
                }
            }
        }
    }
    
    for (int x = 0; x < 10 * n; ++x){
        if (fabs(d[x] - 2 * a[x]) > PRECISION){
            err += 1;
        }
    }

    free(a);
    free(b);
    free(c);
    free(c_copy);
    free(d);
    return err;
}


int main()
{
  int i;                        /* Loop index */
  int result;           /* return value of the program */
  int failed=0;                 /* Number of failed tests */
  int success=0;                /* number of succeeded tests */
  static FILE * logFile;        /* pointer onto the logfile */
  static const char * logFileName = "OpenACC_testsuite.log";        /* name of the logfile */


  /* Open a new Logfile or overwrite the existing one. */
  logFile = fopen(logFileName,"w+");

  printf("######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  printf("## Repetitions: %3d                       ####\n",REPETITIONS);
  printf("## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  printf("##############################################\n");
  printf("Testing parallel_firstprivate\n\n");

  fprintf(logFile,"######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  fprintf(logFile,"## Repetitions: %3d                       ####\n",REPETITIONS);
  fprintf(logFile,"## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  fprintf(logFile,"##############################################\n");
  fprintf(logFile,"Testing parallel_firstprivate\n\n");

  for ( i = 0; i < REPETITIONS; i++ ) {
    fprintf (logFile, "\n\n%d. run of parallel_firstprivate out of %d\n\n",i+1,REPETITIONS);
    if (test() == 0) {
      fprintf(logFile,"Test successful.\n");
      success++;
    } else {
      fprintf(logFile,"Error: Test failed.\n");
      printf("Error: Test failed.\n");
      failed++;
    }
  }

  if(failed==0) {
    fprintf(logFile,"\nDirective worked without errors.\n");
    printf("Directive worked without errors.\n");
    result=0;
  } else {
    fprintf(logFile,"\nDirective failed the test %i times out of %i. %i were successful\n",failed,REPETITIONS,success);
    printf("Directive failed the test %i times out of %i.\n%i test(s) were successful\n",failed,REPETITIONS,success);
    result = (int) (((double) failed / (double) REPETITIONS ) * 100 );
  }
  printf ("Result: %i\n", result);
  return result;
}


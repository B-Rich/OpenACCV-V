#include "acc_testsuite.h"

int test(){
    int err = 0;
    int multiplicitive_n = 128;
    srand(time(NULL));
    real_t * a = (real_t *)malloc(10 * multiplicitive_n * sizeof(real_t));
    real_t * b = (real_t *)malloc(10 * multiplicitive_n * sizeof(real_t));
    real_t * c = (real_t *)malloc(10 * sizeof(real_t));

    for (int x = 0; x < 10 * multiplicitive_n; ++x){
        a[x] = rand() / (real_t) RAND_MAX;
        b[x] = rand() / (real_t) RAND_MAX;
    }

    real_t temp;
    #pragma acc data copyin(a[0:10*multiplicitive_n], b[0:10*multiplicitive_n]) copyout(c[0:10])
    {
        #pragma acc parallel loop private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 1.0;
            #pragma acc loop vector reduction(*:temp)
            for (int y = 0; y < multiplicitive_n; ++y){
                temp *= a[(x * multiplicitive_n) + y] + b[(x * multiplicitive_n) + y];
            }
            c[x] = temp;
        }
    }

    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < multiplicitive_n; ++y){
            c[x] /= a[(x * multiplicitive_n) + y] + b[(x * multiplicitive_n) + y];
        }
    }

    for (int x = 0; x < 10; ++x){
        if (fabs(c[x] - 1) > PRECISION * (4 * multiplicitive_n - 1)){
            err = 1;
        }
    }

    free(a);
    free(b);
    free(c);
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
  printf("Testing parallel_loop_reduction_multiply_vector_loop\n\n");

  fprintf(logFile,"######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  fprintf(logFile,"## Repetitions: %3d                       ####\n",REPETITIONS);
  fprintf(logFile,"## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  fprintf(logFile,"##############################################\n");
  fprintf(logFile,"Testing parallel_loop_reduction_multiply_vector_loop\n\n");

  for ( i = 0; i < REPETITIONS; i++ ) {
    fprintf (logFile, "\n\n%d. run of parallel_loop_reduction_multiply_vector_loop out of %d\n\n",i+1,REPETITIONS);
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


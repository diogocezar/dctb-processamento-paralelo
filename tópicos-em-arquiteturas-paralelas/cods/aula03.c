/* Comparação entre implementações Sequencial, MPI, PThreads e OpenMP
   do produto escalar entre dois vetores
   
   FONTE: Chapman, pg.17-21
   
*/
  

/*  Sequential Dot-Product  
    The sequential program multiplies the individual elements of two arrays
    and saves the result in the variable sum; sum is a so-called reduction
    variable.
*/

int main(argc,argv)
int argc;
char *argv[];
{
  double sum;
  double a [256], b [256];
  int n, i;
  n = 256;
  for (i = 0; i < n; i++) {
     a [i] = i * 0.5;
     b [i] = i * 2.0;
  }
  sum = 0;
  for (i = 0; i < n; i++ ) {
     sum = sum + a[i]*b[i];
  }
  printf ("sum = %f", sum);
}
   
   
/* Dot-Product in MPI 
   Under MPI, all data is local. To implement the dot-product, each process
   builds a partial sum, the sum of its local data. To do so, each executes
   a portion of the original loop. Data and loop iterations are accordingly
   manually shared among processors by the programmer. In a subsequent step,
   the partial sums have to be communicated and combined to obtain the global
   result. MPI provides the global communication routine MPI_Allreduce for
   this purpose.
*/
int main(argc,argv)
int argc;
char *argv[];
{
  double sum, sum_local;
  double a [256], b [256];
  int i, n, numprocs, myid, my_first, my_last;
  n = 256;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  my_first = myid * n/numprocs;
  my_last = (myid + 1) * n/numprocs;
/*  for (i = my_first; i < my_last; i++) { */
  for (i = 0; i < n; i++) {
     a [i] = i * 0.5;
     b [i] = i * 2.0;
  }
  sum_local = 0;
  for (i = my_first; i < my_last; i++) {
        sum_local = sum_local + a[i]*b[i];
  }
  MPI_Allreduce(&sum_local, &sum, 1, MPI_DOUBLE, MPI_SUM,
                                                MPI_COMM_WORLD);
  if (myid==0) printf ("sum = %f", sum);
}


/* Dot-Product in Pthreads
   In the Pthreads programming API, all data is shared but logically distributed
   among the threads. Access to globally shared data needs to be explicitly 
   synchronized by the user. In the dot-product implementation shown, each
   thread builds a partial sum and then adds its contribution to the global sum.
   Access to the global sum is protected by a lock so that only one thread at a
   time updates this variable. We note that the implementation effort in 
   Pthreads is as high as, if not higher than, in MPI.
*/

#define NUMTHRDS 4
double sum;
double a [256], b [256];
int status;
int n=256;
pthread_t thds[NUMTHRDS];
pthread_mutex_t mutexsum;

int main(argc,argv)
int argc;
char *argv[];
{
  int i;
  pthread_attr_t attr;
  for (i = 0; i < n; i++) {
      a [i] = i * 0.5;
      b [i] = i * 2.0;
  }
  pthread_mutex_init(&mutexsum, NULL);
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  for(i=0;i<NUMTHRDS;i++)
  {
     pthread_create( &thds[i], &attr, dotprod, (void *)i);
  }
  pthread_attr_destroy(&attr);
  for(i=0;i<NUMTHRDS;i++) {
     pthread_join( thds[i], (void **)&status);
  }
  printf ("sum = %f \n", sum);
  pthread_mutex_destroy(&mutexsum);
  pthread_exit(NULL);
}

void *dotprod(void *arg)
{
    int myid, i, my_first, my_last;
    double sum_local;
    myid = (int)arg;
    my_first = myid * n/NUMTHRDS;
    my_last = (myid + 1) * n/NUMTHRDS;
    sum_local = 0;
    for (i = my_first; i < my_last; i++) {
      sum_local = sum_local + a [i]*b[i];
    }
    pthread_mutex_lock (&mutexsum);
    sum = sum + sum_local;
    pthread_mutex_unlock (&mutexsum);
    pthread_exit((void*) 0);
}


/*  Dot-Product in OpenMP 
    Under OpenMP, all data is shared by default. In this case, we are able to
    parallelize the loop simply by inserting a directive that tells the compiler
    to parallelize it, and identifying sum as a reduction variable. The details
    of assigning loop iterations to threads, having the different threads build
    partial sums and their accumulation into a global sum are left to the
    compiler. Since (apart from the usual variable declarations and
    initializations) nothing else needs to be specified by the programmer,
    this code fragment illustrates the simplicity that is possible with OpenMP.
*/
int main(argc,argv)
int argc; char *argv[];
{
  double sum;
  double a [256], b [256];
  int status;
  int n=256;
  for (i = 0; i < n; i++) {
      a [i] = i * 0.5;
      b [i] = i * 2.0;
  }
  sum = 0;
  #pragma omp parallel for reduction(+:sum)
  for (i = 1; i <= n; i++ ) {
      sum = sum + a[i]*b[i];
  }
  printf ("sum = %f \n", sum);
}









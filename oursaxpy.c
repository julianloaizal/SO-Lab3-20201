/**
 * @defgroup   SAXPY saxpy
 *
 * @brief      This file implements an iterative saxpy operation
 * 
 * @param[in] <-p> {vector size} 
 * @param[in] <-s> {seed}
 * @param[in] <-n> {number of threads to create} 
 * @param[in] <-i> {maximum itertions} 
 *
 * @author     Danny Munera
 * @date       2020
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct __buf_t {
    int min;
    int max;
} buf_t;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

double *X, *Y, *Y_avgs;

double a;

int it, p;

void *
saxpy (void *arg)
{
  int i, max;
  buf_t *buf;
  double tmp;

  buf = (buf_t *) arg;
  i   = buf->min;
  max = buf->max;
  tmp = 0.0;
  while (i < max)
    {
      Y[i] = Y[i] + a * X[i];
      tmp += Y[i];
      ++i;
    }
  pthread_mutex_lock (&lock);
  Y_avgs[it] += tmp / p;
  pthread_mutex_unlock (&lock);
  free (buf);
  return NULL;
}

int
main (int argc, char *argv[])
{
  // Variables to obtain command line parameters
  unsigned int seed = 1;
  p = 10000000;
  int n_threads = 2;
  int max_iters = 1000;
  // Variables to perform SAXPY operation
  int i;
  // Variables to get execution time
  struct timeval t_start, t_end;
  double exec_time;

  // Getting input values
  int opt;
  while ((opt = getopt (argc, argv, ":p:s:n:i:")) != -1)
    {
      switch (opt)
	{
	case 'p':
	  printf ("vector size: %s\n", optarg);
	  p = strtol (optarg, NULL, 10);
	  assert (p > 0 && p <= 2147483647);
	  break;
	case 's':
	  printf ("seed: %s\n", optarg);
	  seed = strtol (optarg, NULL, 10);
	  break;
	case 'n':
	  printf ("threads number: %s\n", optarg);
	  n_threads = strtol (optarg, NULL, 10);
	  break;
	case 'i':
	  printf ("max. iterations: %s\n", optarg);
	  max_iters = strtol (optarg, NULL, 10);
	  break;
	case ':':
	  printf ("option -%c needs a value\n", optopt);
	  break;
	case '?':
	  fprintf (stderr,
		   "Usage: %s [-p <vector size>] [-s <seed>] [-n <threads number>]\n",
		   argv[0]);
	  exit (EXIT_FAILURE);
	}
    }
  srand (seed);

  printf ("p = %d, seed = %d, n_threads = %d, max_iters = %d\n",
	  p, seed, n_threads, max_iters);

  // initializing data
  X = (double *) malloc (sizeof (double) * p);
  Y = (double *) malloc (sizeof (double) * p);
  Y_avgs = (double *) malloc (sizeof (double) * max_iters);

  for (i = 0; i < p; i++)
    {
      X[i] = (double) rand () / RAND_MAX;
      Y[i] = (double) rand () / RAND_MAX;
    }
  for (i = 0; i < max_iters; i++)
    {
      Y_avgs[i] = 0.0;
    }
  a = (double) rand () / RAND_MAX;

#ifdef DEBUG
  printf ("vector X= [ ");
  for (i = 0; i < p - 1; i++)
    {
      printf ("%f, ", X[i]);
    }
  printf ("%f ]\n", X[p - 1]);

  printf ("vector Y= [ ");
  for (i = 0; i < p - 1; i++)
    {
      printf ("%f, ", Y[i]);
    }
  printf ("%f ]\n", Y[p - 1]);

  printf ("a= %f \n", a);
#endif

  /*
   *      Function to parallelize 
   */

  pthread_t tid[n_threads];
  int m = p / n_threads;

  gettimeofday (&t_start, NULL);
  //SAXPY iterative SAXPY mfunction
  for (it = 0; it < max_iters; it++)
    {
      for (i = 0; i < n_threads; i++)
	{
	  buf_t *buf = malloc (sizeof (int) * 2);
      buf->min = i * m;
	  buf->max = ((i + 1) == n_threads) ? p : buf->min + m;
	  pthread_create (&tid[i], NULL, saxpy, buf);
	  /* 
	   * free(buf) is doing inside of saxpy()
	   */
	}
      for (i = 0; i < n_threads; i++)
	{
	  pthread_join (tid[i], NULL);
	}
    }
  gettimeofday (&t_end, NULL);

#ifdef DEBUG
  printf ("RES: final vector Y= [ ");
  for (i = 0; i < p - 1; i++)
    {
      printf ("%f, ", Y[i]);
    }
  printf ("%f ]\n", Y[p - 1]);
#endif

  // Computing execution time
  exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;	// sec to ms
  exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0;	// us to ms
  printf ("Execution time: %f ms \n", exec_time);
  printf ("Last 3 values of Y: %f, %f, %f \n", Y[p - 3], Y[p - 2], Y[p - 1]);
  printf ("Last 3 values of Y_avgs: %f, %f, %f \n", Y_avgs[max_iters - 3],
	  Y_avgs[max_iters - 2], Y_avgs[max_iters - 1]);
  return 0;
}

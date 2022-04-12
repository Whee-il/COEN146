// Name: Will Cockrum 
// Date: 4/1/21
// Title: Lab1 – Threads 
// Description: Demonstrates using a thread

/*Sample C program for Lab assignment 1*/
#include <stdio.h>      /* printf, stderr */
#include <sys/types.h>  /* pid_t */
#include <unistd.h>     /* fork */
#include <stdlib.h>     /* atoi */
#include <errno.h>      /* errno */ 
#include <pthread.h>    /* pthread_create */
/* main function with command-line arguments to pass */
void *start_routine( void *ptr );
main(int argc, char *argv[])
{
    pthread_t thread1;
    int i, n = (int) atoi(argv[1]);
    int iret1;
    /* Create independent threads each of which will execute function */
    iret1 = pthread_create( &thread1, NULL, start_routine,
    (void*) &n);

    /* Wait till threads are complete before main continues. */
    /* wait we run the risk of executing an exit which will terminate */
    /* the process and all threads before the threads have completed */

    for (i=0;i<100;i++) {
            printf("\t \t \t Parent Thread %d \n",i);
            usleep(n);
        }
    pthread_join( thread1, NULL);

    exit(0);
}
void *start_routine( void *ptr )
{
    
    int n, i;
    n = *(int*)ptr;

    for (i=0;i<100;i++) {
            printf("Created Thread %d\n",i);
            usleep(n);
        }

}

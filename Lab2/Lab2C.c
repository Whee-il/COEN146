// Name: Will Cockrum 
// Date: 4/1/21
// Title: Lab2 – Multithread Copying 
// Description: Copying Multiple files at the same time with threads

#include <stdio.h>      
#include <sys/types.h>  
#include <unistd.h>     
#include <stdlib.h>     
#include <errno.h>      
#include <pthread.h>    /* pthread_create *

/* main function with command-line arguments to pass */
void *thread_copy(NULL void *ptr );
main(int argc, char *argv[])
{
    pthread_t thread1,thread2,thread3,thread4,thread5;
    int copy1, copy2, copy3, copy4, copy5;

    int n1 = 1;
    int n2 = 2;
    int n3 = 3;
    int n4 = 4;
    int n5 = 5;
    
    /* Create independent threads each of which will execute function */

    copy1 = pthread_create( &thread1, NULL, thread_copy,(void*) &n1);
    copy2 = pthread_create( &thread2, NULL, thread_copy,(void*) &n2);
    copy3 = pthread_create( &thread3, NULL, thread_copy,(void*) &n3);
    copy4 = pthread_create( &thread4, NULL, thread_copy,(void*) &n4);
    copy5 = pthread_create( &thread5, NULL, thread_copy,(void*) &n5);

    /* Wait till threads are complete before main continues. */
    /* wait we run the risk of executing an exit which will terminate */
    /* the process and all threads before the threads have completed */

    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
    pthread_join( thread3, NULL);
    pthread_join( thread4, NULL);
    pthread_join( thread5, NULL);

    exit(0);
}
void *thread_copy( void *ptr )
{
    int *number = (int*) ptr;
    char number_c = '0' + *number;
    char src_name[9] = "rand0.dat"; 
    char dest_name[9] = "copy0.dat"; 
    src_name[4] = number_c;
    dest_name[4] = number_c;
    

    
    FILE * src = fopen(src_name, "r");
	FILE * dest = fopen(dest_name, "w");

    char buffer[100];

    int bytes_read;

    if (src == NULL){
		printf("Error opening source\n");
		return 0;
	}

	if (dest == NULL){
		printf("Error opening destination\n");
		return 0;
	}

    while (!feof(src)){
		bytes_read = fread(&buffer, 1, sizeof(buffer), src);
		fwrite(&buffer, 1, bytes_read, dest);
	}
	fclose(src);
	fclose(dest);

    return;

}

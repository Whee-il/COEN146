//Will Cockrum
//April 8 2021
//Lab 2 Part 2
//Copies files using system calls

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <time.h>


int main (int argc, char * argv[]){
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    int src = open(argv[1], O_RDWR);
    int dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);

    char buffer[100];
    int bytes_read;

    if (src == -1){
		printf("Error opening source\n");
		return 0;
	}

	if (dest == -1){
		printf("Error opening destination\n");
		return 0;
	}

    while (1){
        int bytes_read = read(src, buffer, sizeof(buffer));
        if (bytes_read == 0){
            break;
        }
        int bytes_written = write(dest, buffer, bytes_read);
	}
	close(src);
	close(dest);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Program took %f time\n", cpu_time_used);
    
    return 0;
}

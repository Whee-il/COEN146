//Will Cockrum
//April 8 2021
//Lab 2 Part 1
//Copies files using function calls

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main (int argc, char * argv[]){
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    FILE * src = fopen(argv[1], "r");
	FILE * dest = fopen(argv[2], "w");

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
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Program took %f time\n", cpu_time_used);
    return 0;
}

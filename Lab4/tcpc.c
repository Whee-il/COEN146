//Lab4: Client

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h> 

int main (int argc, char *argv[]) {
    int		sockfd = 0, n = 0;
    char	buff[1024];
    struct	sockaddr_in servAddr; 
    FILE 	*fp;

    if (argc != 4){
	    printf ("Usage: %s <port no>  <ip of server> <src_file> \n",argv[0]);
	    exit (1);
    } 

        //Open file
    if (((fp=fopen(argv[3],"r"))) == NULL) {
		perror("Failure to open the file");
        exit(1);
    }

    //Create socket
    printf("creating socket\n\n");
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Failure to setup an endpoint socket");
        exit(1);
    } 
    printf("socket created\n\n");
    //Set server address by filling sevAddr fields
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons (atoi (argv[1]));  
    printf("port\n\n");
    servAddr.sin_addr.s_addr = inet_addr(argv[2]) ;
    //*((struct in_addr *)host->h_addr);
    printf("addr\n\n");

    //Connet to server
    if (connect(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr)) < 0){
		perror("Failure to connect to the server");
        exit(1);
    } 
    printf("connected to server\n\n");


    int count=1;
    //read from file and write to server
    //your code
    int bytes_read;
    while (!feof(fp)){
		bytes_read = fread(&buff, 1, sizeof(buff), fp);
        printf("Sending %s %d\n\n", buff, bytes_read);
        write(sockfd, buff, bytes_read);
	}

    fclose(fp);
    close(sockfd);
    return 0;
}

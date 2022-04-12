 //Lab4: Concurrent server  

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>

struct d{ 		//structure to hold thread data
   int con; //connection
   FILE *fp; //file pointer
   int tID; //thread id
};

int main (int argc, char *argv[]){
    if (argc != 3){
		  printf ("Usage: %s <port number> <src_file> \n",argv[0]);
		  exit(1);
	  }
    int		sockfd;             //Declare socket file descriptor
    char	buff[1024];           //Declare a buffer of size 1k bytes
    int     connfd;    // connection array
    FILE *fp;
    
    //Declare server address to which to bind for receiving messages and client address to fill in sending address
    struct	sockaddr_in servAddr, clienAddr; 
    
    //Set server address/ port
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servAddr.sin_port = htons (atoi (argv[1])); 
    
    //Open a TCP socket, if successful, returns a descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
      perror("Failure to setup an endpoint socket");
      exit(1);
    }
    
    //Set address/port of server endpoint for socket descriptor
    if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0){
      perror("Failure to bind server address to the endpoint socket");
      exit(1);
    } 

    printf("Server waiting for clients at port %d\n", servAddr.sin_port);

    // Server listening to the socket endpoint, and can queue 10 client requests
    //your code 
    listen(sockfd, 10);


    
    
    int sin_size = sizeof(struct sockaddr_in); //size of sockaddr_in
    
    //Server loops all the time accepting conncections when they are made, then passing connfd to a thread
    while (1) {

        //Server accepting to establish a connection with a connecting client, if successful, returns a connection descriptor
        if ((connfd = accept(sockfd, (struct sockaddr *)&clienAddr, (socklen_t *)&sin_size)) < 0){
          perror("Failure to accept connection to the client");
          exit(1);
        }
        //Connection established, server begins to read and write to the connecting client
        printf("Connection Established with client: IP %s and Port %d\n", inet_ntoa(clienAddr.sin_addr), ntohs(clienAddr.sin_port));
        char buffer[1024];
        int bytes_read;
        fp=fopen((argv[2]),"w");
        if (fp == NULL){
          perror("Failure to open the file");
          exit(1);
        } 
        do{

            bytes_read = read(connfd, buffer, 1024);
            //perror("Failure to read info");
            printf("writing %s %d \n\n", buffer, bytes_read);

            //fprintf(data->fp, "%s", buffer);
            //fprintf(data->fp, "test\n");
            int bytes_written = fwrite(&buffer, 1, bytes_read, fp);
            printf("wrote %d\n\n", bytes_written);
            
        }while(bytes_read > 0); 
        fclose(fp);
        
    }
    
    close(sockfd);

    return 0;
}

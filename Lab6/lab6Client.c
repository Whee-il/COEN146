//Lab5 Client
#include "lab6.h"

/*The simplest checksum algorithm is the so-called longitudinal parity check, 
which breaks the data into "bytes" with a fixed number 8 of bits, and then 
computes the exclusive or (XOR) of all those bytes. 
The result is appended to the message as an extra byte*/
//getChecksum()
int getChecksum(Packet packet) {
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
    while (ptr < end) {
        checksum ^= *ptr++;
    } 
        return checksum;
}
// printPacket()
void printPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}
//clientSend()
void clientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet, unsigned retries) {
    while (1) {	
        if(retries >= 3){
            break;
        }
		packet.header.cksum=getChecksum(packet);
        // send the packet
        printf("Client sending packet\n");
	    //your code to send packet
        if (rand() % PLOSTMSG == 0) {
            printf("Dropping packet\n");
        } else {
            if(sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)address, addrlen) == -1){
                perror("Client: Error sending packet");
                exit(1);
            }
        }
        // wait until an ACK is received or timeout using select statement
        // recall recvfrom() is a blocking function, i.e. blocks until data is available
        // so what if data from server got lost, so select() solves this problem. type man select for more information.

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int rv;
        //setup select parameters
        fd_set readfds;
        fcntl(sockfd,F_SETFL,O_NONBLOCK);
        //start before calling select
        FD_ZERO(&readfds);
        FD_SET(sockfd,&readfds);
        //call select. sockfd+1 is used because select checks upto sockfd-1 descriptors.
        rv=select(sockfd+1,&readfds,NULL,NULL,&tv);

        if(rv==0){ // no data has been received
            printf("Timeout\n");
            //increment retries if packet is dropped
            retries++;
        }
        else{
            // receive an Ack from the server
            Packet recvpacket;
            //your code to receive packet/Ack
            if (recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)address, &addrlen) == -1){
                perror("Client: Error recieving ACK");
                exit(1);
            }
            // print what was received
            printf("Client received ACK %d, checksum %d - \n", recvpacket.header.seq_ack, recvpacket.header.cksum);
            // your code to validate the ACK
            int e_cksum = getChecksum(recvpacket);
            if (recvpacket.header.cksum!=e_cksum ) {
                // bad checksum, resend packet
                printf("Client: Bad checksum, expected checksum was: %d\n", e_cksum);
                if(sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)address, addrlen) == -1){
                    perror("Client: Error sending packet");
                    exit(1);
                }
            } else if (recvpacket.header.seq_ack!=packet.header.seq_ack) {
                // incorrect sequence number, resend packet
                printf("Client: Bad seqnum, expected sequence number was: %d\n",packet.header.seq_ack);
                if(sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)address, addrlen) == -1){
                    perror("Client: Error sending packet");
                    exit(1);
                }
            } else {
                // good ACK, we're done
                printf("Client: Good ACK\n");
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <infile>\n", argv[0]);
        return 1;
    }
    //Seed the random function
    srand((unsigned)time(NULL));

    // your code to create and configure the socket [name the socket sockfd] 
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
	    perror("Failed to setup endpoint socket.\n");
	    exit(1);	
    }

    // your code to initialize server address structure using argv[2] and argv[1]
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi (argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    socklen_t addrlen = sizeof(server_addr);
    // your code to open file using argv[3]
    FILE *fp;
    if ((fp=fopen(argv[3],"r")) == NULL) {
		perror("Failure to open the file");
        exit(1);
    }

    // your code to send file contents to server
    int seq=0;
    Packet pack;
    while(!feof(fp)){
        pack.header.len = fread(&pack.data, 1, SIZE, fp);
    	pack.header.seq_ack=seq;
    	//pack.header.len=strlen(pack.data);
        pack.header.cksum = getChecksum(pack);
        
        clientSend(sockfd, (struct sockaddr *)&server_addr, addrlen, pack,0);
    	seq=(seq+1)%2;
    }

    // send zero-length packet to server to end connection
    Packet final;
    final.header.seq_ack=seq;
    final.header.len=0;
    final.header.cksum=getChecksum(final);
    clientSend(sockfd,(struct sockaddr *)&server_addr,addrlen,final,0);
    // clean up
	//-close the sockets and file
    fclose(fp);
    close(sockfd);
    return 0;
}

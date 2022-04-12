
//Lab5 Server
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

//server sending ACK to the client
void serverSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
        // Simulating a chance that ACK gets lost
    if (rand() % PLOSTMSG == 0) {
        printf("Dropping ACK\n");
    }
    else{
        Packet packet;	
        packet.header.seq_ack=seqnum;
        packet.header.len=sizeof(packet.data);
        packet.header.cksum=getChecksum(packet);
        //send packet
        //your code to send packet/Ack
        printf("\t Server sending ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);

        if(sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)address, addrlen) == -1){
            perror("Error sending ACK");
            exit(1);
        }
    }
}
//server receiving packet
Packet serverReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) {
    Packet packet;
    int e_cksum;
    while (1) {
        //your code to receive packets from the client
        if (recvfrom(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)address, addrlen) == -1){
			perror("Error recieving data");
			exit(1);
		}

        e_cksum = getChecksum(packet);
        // print received packet
        printPacket(packet);
        // your code to validate the packet
        if (packet.header.cksum != e_cksum) {
            printf("\t Server: Bad checksum, expected checksum was: %d\n", e_cksum);
            serverSend(sockfd, address, *addrlen, !seqnum);
        } else if (packet.header.seq_ack != seqnum) {
            printf("\t Server: Bad seqnum, expected sequence number was:%d\n",seqnum);
            serverSend(sockfd, address, *addrlen, !seqnum);
        } else {
            printf("\t Server: Good packet\n");
            serverSend(sockfd, address, *addrlen, seqnum);

            break;
        }
    }
    return packet;
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <outfile>\n", argv[0]);
        exit(1);
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    // create to create and configure the socket
    int sockfd;
    struct sockaddr_in server_addr;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
	    perror("failure to setup an endpoint socket\n");	
	    exit(1);
    }

    // your code to initialize the server address structure using argv[1]
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi (argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd,(struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    	{
        	perror("failed to bind socket");
        	exit(1);
    	}
    printf("\nServer Waiting for client on port %s\n",argv[1]);
    // your code to open file using argv[2]
    FILE *fp = fopen(argv[2], "w");
	if (fp == NULL){
	    perror("File failed to open\n");
	    exit(1);
    }

    // your code to get file contents from client and write it to the file
    int seqnum = 0;
    Packet packet;
    struct sockaddr_in clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);

    do {
        packet = serverReceive(sockfd, (struct sockaddr *)&clientaddr, &clientaddr_len, seqnum);

        fwrite(packet.data,1,packet.header.len,fp);
    	seqnum=(seqnum+1)%2;
    } while (packet.header.len != 0);

    fclose(fp);
    close(sockfd);
    return 0;
}

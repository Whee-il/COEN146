// Lab7: Link State routing
 
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>


//defines
#define	N			4
#define	INFINITE	1000
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))


// types
typedef struct machines
{
	char	name[50];
	char	ip[50];
	int		port;	

} MACHINES;


// global variables
MACHINES	machines[N];
int			costs[N][N];
int			distances[N];
int			myid;
int			sock;
struct sockaddr_in addr;
struct sockaddr_in otheraddr;
socklen_t addr_size;
pthread_mutex_t	lock;


// functions
void *run_link_state (void *);
void *receive_info (void *);
void print_costs (void);


//main()
int main (int argc, char *argv[])
{
	if (argc != 4) {
		fprintf(stderr, "usage: %s <myid> <costs> <machines>\n", argv[0]);
		exit(1);
	}
	FILE	*fp;
	int		i, j;
	pthread_t	thr1, thr2;
	int		id, cost;
	int		packet[3];

	
	myid 	= 	atoi(argv[1]);
	// get costs
	if ((fp = fopen (argv[2], "r")) == NULL)
	{
		printf ("can't open %s\n", argv[2]);
		return 1;
	}
	
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			fscanf (fp, "%d", &costs[i][j]);
		}
	}

	fclose (fp);
		

	//get info on machines 
	if ((fp = fopen (argv[3], "r")) == NULL)
	{
		printf ("can't open %s\n", argv[3]);
		return 1;
	}

	for (i = 0; i < N; i++)
		fscanf (fp, "%s%s%d", machines[i].name, machines[i].ip, &machines[i].port);

	fclose (fp);

	
    // init address
    addr.sin_family = AF_INET;
    addr.sin_port = htons ((short)machines[myid].port);
    addr.sin_addr.s_addr = htonl (INADDR_ANY);
    memset ((char *)addr.sin_zero, '\0', sizeof (addr.sin_zero));
    addr_size = sizeof (addr);

    // create socket
    if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf ("socket error\n");
        return 1;
    }

    // bind
    if (bind (sock, (struct sockaddr *)&addr, sizeof (addr)) != 0)
    {
        printf ("bind error\n");
        return 1;
    }
	

	// create threads
	pthread_mutex_init (&lock, NULL);
	pthread_create (&thr1, NULL, receive_info, NULL);
	pthread_create (&thr2, NULL, run_link_state, NULL);

	// read changes from the keyboard
	for (i = 0; i < 3; i++)
	{
		printf ("any changes? ");
		scanf ("%d%d", &id, &cost);
		if (id >= N  ||  id == myid)
		{
			printf ("wrong id\n");
			break;
		}

		pthread_mutex_lock (&lock);
		costs[myid][id] = cost;
		costs[id][myid] = cost;
		pthread_mutex_unlock (&lock);
		print_costs ();

		packet[0] = htonl (myid);
		packet[1] = htonl (id);
		packet[2] = htonl (cost);
		otheraddr.sin_family = AF_INET;
		addr_size = sizeof (otheraddr);

		for (j = 0; j < N; j++)
		{
			if (j != myid)
			{
				otheraddr.sin_port = htons ((short)machines[j].port);
				inet_pton (AF_INET, machines[j].ip, &otheraddr.sin_addr.s_addr);
				sendto (sock, packet, sizeof (packet), 0, (struct sockaddr *)&otheraddr, addr_size);
			}
		}
		printf ("sent\n");
	}

	sleep (20);

}


// receive info
void *
receive_info (void *arg)
{
	//Code to receive info and update costs
	int self;
	int other;
	int cost;
	int packet[3];

	while (1){
		// Receive updated link cost
		if (recv(sock, &packet, sizeof(packet), 0) == -1){
			perror("Client: Error recieving Packet");
			exit(1);
		}

		self = ntohl(packet[0]);
		other = ntohl(packet[1]);
		cost = ntohl(packet[2]);		

		// Assign to costs table
		pthread_mutex_lock(&lock);
		costs[self][other] = cost;
		costs[other][self] = cost;
		pthread_mutex_unlock(&lock);
		
	}

}


// run_link_state
void *
run_link_state (void *arg)
{
	int	taken[N];
	int	min, spot;
	int	i, j;
	int	r;

	while (1)
	{
		r = rand () % 10;
		sleep (r);
	
		for (i = 0; i < N; i++)
		{
			taken[i] = 0;
			pthread_mutex_lock (&lock);
			distances[i] = costs[myid][i];
			pthread_mutex_unlock (&lock);
		}
		taken[myid] = 1;
	
		for (i = 1; i < N; i++)
		{
			// find closest node
			spot = 0;
			min = 100;
			for (j = 0; j < N; ++j){
				if(taken[j] == 0 && distances[j] < min){
					min = distances[j];
					spot = j;
				}
			}
			taken[spot] = 1;

			
			// recalculate distances
			for (j = 0; j < N; j++)
			{
				if (taken[j] == 0)
				{
					pthread_mutex_lock (&lock);
					distances[j] = MIN (distances[j], (distances[spot] + costs[spot][j]));
					pthread_mutex_unlock (&lock);
				}
			}
		}

		printf ("new distances:\n");
		for (i = 0; i < N; i++)
			printf ("%d ", distances[i]);
		printf ("\n");
	}
}


// print costs 
void
print_costs (void)
{
	int i, j;
	for (i = 0; i < N; ++i){
		for (j = 0; j < N; ++j){
			printf("%d\t", costs[i][j]);
		}
		printf("\n\n");
	}

}
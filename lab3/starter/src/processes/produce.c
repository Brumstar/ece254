// Use this to see if a number has an integer square root
#define EPS 1.E-7


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <sys/fcntl.h>
#include <fcntl.h>
#include <mqueue.h>
#include <errno.h>
double g_time[2];


int main(int argc, char *argv[])
{
	int num;
	int maxmsg;
	int num_p;
	int num_c;
	int errno;
	struct timeval tv;
	mqd_t queue_d;
	pid_t consumer_spawner_pid;

	if (argc != 5) {
		printf("Usage: %s <N> <B> <P> <C>\n", argv[0]);
		exit(1);
	}

	num = atoi(argv[1]);	/* number of items to produce */
	maxmsg = atoi(argv[2]); /* buffer size                */
	num_p = atoi(argv[3]);  /* number of producers        */
	num_c = atoi(argv[4]);  /* number of consumers        */

	gettimeofday(&tv, NULL);
        g_time[0] = (tv.tv_sec) + tv.tv_usec/1000000.;
	queue_d = mq_open("/coolqueue", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, NULL);	
	printf("Queue desc %d\n", queue_d);
	printf("Doing first fork of consumers\n");
	consumer_spawner_pid = fork();

	if (consumer_spawner_pid > 0) {
		// parent
	} else if (consumer_spawner_pid == 0) {
		printf("First child\n");
		pid_t cons_pids[num_c];
		for (int i = 0; i < num_c; i++) {
			printf("I'm spawning consumers\n");
			cons_pids[i] = fork();
			printf("Forked pid is %d\n", cons_pids[i]);
			if (cons_pids[i] == 0) {
				printf("Consumer\n");
			} else {
				return 0;
			}
		}
	} else {
		return -1;
	}


    	gettimeofday(&tv, NULL);
    	g_time[1] = (tv.tv_sec) + tv.tv_usec/1000000.;

    	printf("System execution time: %.6lf seconds\n", \
            g_time[1] - g_time[0]);
	exit(0);
}




#define EPS 1.E-7


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <fcntl.h>
#include <sys/fcntl.h>

double g_time[2];


int main(int argc, char *argv[])
{
	int num;
	int maxmsg;
	int num_p;
	int num_c;
	int i;
	struct timeval tv;
	pid_t pini;
	
	mqd_t queue_d;
	struct mq_attr attr;


	if (argc != 5) {
		printf("Usage: %s <N> <B> <P> <C>\n", argv[0]);
		exit(1);
	}

	num = atoi(argv[1]);	/* number of items to produce */
	maxmsg = atoi(argv[2]); /* buffer size                */
	num_p = atoi(argv[3]);  /* number of producers        */
	num_c = atoi(argv[4]);  /* number of consumers        */

	mq_unlink("/coolqueue");
 	attr.mq_maxmsg = 10;
    attr.mq_msgsize = 20;
    queue_d = mq_open("/coolqueue", O_RDWR | O_CREAT, 0644, &attr);
    printf("Open desc %d\n", queue_d);
    mqd_t send = mq_open("/coolqueue", O_WRONLY);
    printf("Send desc %d\n", send);
    mq_send(queue_d, "1\0", strlen("1\0"), 1);
    mqd_t recv = mq_open("/coolqueue", O_RDONLY);
    printf("Recv desc %d\n", recv);
    char rcv[40];
    mq_receive(recv, rcv, 40, NULL);
    perror(NULL);
    int received = atoi(rcv);
    printf ("Received int %d", received);

	pini = fork();
	if (pini<0){
		return -1;
	}else if ( pini == 0){
    	/*spawner p*/
    	printf("Producer spawner\n");
    	pid_t pros_pids[num_p];

    	for(int i = 0; i < num_p; i++){
    		printf("I'm spawning producers\n");
    		pros_pids[i] = fork();
    		printf("Forked pros_pid is %d\n", pros_pids[i]);
            if (pros_pids[i] == 0) {
                printf("Producer\n");
            } else {
                return 0;
            }
    	}
	}
    else {
    	pid_t cini;
    	cini = fork();
    	if (cini<0){
    		return -1;
    	}else if (cini == 0){
    		/*spwaner c*/
    		printf("Consumer spwaner\n");
            pid_t cons_pids[num_c];

            for (int i = 0; i < num_c; i++) {
                printf("I'm spawning consumers\n");
                cons_pids[i] = fork();
                printf("Forked cons_pid is %d\n", cons_pids[i]);
                if (cons_pids[i] == 0) {
                    printf("Consumer\n");
		    consumer();
                } else {
                    return 0;
                }
            }
    
    	}else{
    		/*parent main*/
    	}
    }


	gettimeofday(&tv, NULL);
	g_time[0] = (tv.tv_sec) + tv.tv_usec/1000000.;


    gettimeofday(&tv, NULL);
    g_time[1] = (tv.tv_sec) + tv.tv_usec/1000000.;

    printf("System execution time: %.6lf seconds\n", \
            g_time[1] - g_time[0]);
	exit(0);
}

void consumer() {
    mqd_t recv = mq_open("/coolqueue", O_RDONLY);
    printf("Recv desc %d\n", recv);
    char rcv[2048];
    mq_receive(recv, rcv, 2048, NULL);
    printf("Received %s\n", rcv);
}

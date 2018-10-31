
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
int num;
int maxmsg;
int num_p;
int num_c;
int consumer_id;
struct timeval tv;
pid_t pini;
mqd_t queue_d;
struct mq_attr attr;

void producer(int);
void consumer(mqd_t fd);

int main(int argc, char *argv[])
{
	


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

   
	pini = fork();
	if (pini<0){
		return -1;
	}else if ( pini == 0){
    	/*spawner p*/
    	pid_t pros_pids[num_p];

    	for(int i = 0; i < num_p; i++){
    		pros_pids[i] = fork();
            if (pros_pids[i] == 0) {
                producer(i);
            } else {
                return 0;
            }
    	}
    } else {
    	pid_t cini;
    	cini = fork();
    	if (cini<0){
    		return -1;
    	} else if (cini == 0){
    		/*spwaner c*/
            pid_t cons_pids[num_c];

            for (int i = 0; i < num_c; i++) {
                cons_pids[i] = fork();
                if (cons_pids[i] == 0) {
                    consumer_id = i;
                    mqd_t recv = mq_open("/coolqueue", O_RDONLY);
                    consumer(recv);
                }
            }
    
    	}else{
    	    gettimeofday(&tv, NULL);
    	    g_time[0] = (tv.tv_sec) + tv.tv_usec/1000000.;


    	    gettimeofday(&tv, NULL);
    	    g_time[1] = (tv.tv_sec) + tv.tv_usec/1000000.;

    	    printf("System execution time: %.6lf seconds\n", \
            g_time[1] - g_time[0]);
    	    exit(0);
	}
    }
}

void producer(int i){	
    mqd_t send = mq_open("/coolqueue", O_WRONLY);

    char msg[num];
    for (int j = i; j < num; j += num_p){
    	sprintf(msg,"%d",j);
    	mq_send(queue_d, msg, strlen(msg), 1);
    }

}

void consumer(mqd_t fd) {
    int received;
    int root;
    char rcv[40];
    while(1) {
        mq_receive(fd, rcv, 40, NULL);
        received = atoi(rcv);
        root = sqrt(received);
        if ((root * root) == received) {
                printf("%d %d %d\n", consumer_id, received, root);
		if (received == num - 1) {
			return;
		}
        }
   	
    }
}

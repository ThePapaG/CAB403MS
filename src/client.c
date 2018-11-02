//CAB403 Assignment 2018 Sem2 - Client main
//Written by Grant Dare n9476512
//in conjunction with Callum Scott

#include "client.h"

void killClient(int sig){
	printf("caught signal");
	send(sockfd, DISCONNECT_SIGNAL, BUF_SIZE, 0);
    pthread_join(writeth, NULL);
    pthread_join(readth,NULL);
	close(sockfd);
	exit(sig);
}

int main(int argc, char* argv[]){
	signal(SIGINT, killClient);	
	//process the input arguments
	if (argc != 3) {
		fprintf(stderr,"usage: client_hostname port_number\n");
		exit(1);
	}

	//Connect to the server
	if ((he=gethostbyname(argv[1])) == NULL) {
		perror("gethostbyname");
		exit(1);
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(atoi(argv[2]));
	host_addr.sin_addr = *((struct in_addr *)he->h_addr);
	bzero(&(host_addr.sin_zero), 8);
	
	if (connect(sockfd, (struct sockaddr *)&host_addr, \
	sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}

    printf("Server connection established\n");

	//split the users sending and receiving on threads
	if (pthread_create(&writeth, NULL, Send, (void *) &sockfd) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&readth, NULL, Rec, (void *) &sockfd) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(writeth, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(readth, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

	/* Close socket. */
    close(sockfd);

    exit(EXIT_SUCCESS);
}

static void *Send(void *data){
    printf("Outgoing thread established\n");
    int     *sock_fd;
    char    send_buf[BUF_SIZE];
    int     input_len;

    sock_fd = (int *) data;

    while (cont) {
        input_len = getSelection("", send_buf);

        if (send(*sock_fd, send_buf, input_len, 0) != input_len) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    pthread_exit(NULL);
}

static void *Rec(void *data){
    printf("Incoming thread established\n");
    int     *sock_fd;
    char    recv_buf[BUF_SIZE];

    sock_fd = (int *) data;

    while (cont) {
        if (recv(*sock_fd, recv_buf, BUF_SIZE, 0) == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (strcmp(recv_buf, DISCONNECT_SIGNAL) == 0) {
            printf("\nReceived disconnect from server.\n");
            killClient(EXIT_SUCCESS);
            break;
        }

        printf("%s", recv_buf);
        fflush(stdout);
    }

    pthread_exit(NULL);
}

int getSelection(char *msg, char *input_str){
    int input_len;
    char *new_line;

    printf("%s", msg);
    fgets(input_str, BUF_SIZE, stdin);

    if ((new_line = strchr(input_str, '\n')) != NULL) {
        *new_line = '\0';
    } else {
        while (getchar() != '\n') { ; }
    }

    input_len = strlen(input_str) + 1;

    return input_len;
}
//CAB403 Assignment 2018 SEM2 - Server
//Written by Callum Scott n9695443
//in conjunction with Grant Dare n9476512

/* Include */

#include "server.h"

void handle_sigint(int sig) { 
    printf("Caught signal %d\n", sig);
    printf("Exiting has begun.");
    close(sockfd);
    exit(1);
} 
	
int CreateSocket(){
	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Check for error */
	if (sockfd==-1)
	{
		perror("error socket");
		exit(1);
	}
	return sockfd;
}

void GenrateEP(){
	/* Generate end point */	
	my_addr.sin_family = AF_INET;         /* host byte order */
	my_addr.sin_port = htons(PORT);     /* short, network byte order */
	my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
}

void BindListen(int sockfd){
	/* Bind socket to end point */
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) \
	== -1) {
		perror("error bind");
		exit(1);
	}

	/* Start listnening */
	if (listen(sockfd, BACKLOG) == -1) {
		perror("error listen");
		exit(1);
	}

	printf("server starts listnening ...\n");
}

void* ClientGame(void *arg){
	int sock = *(int*)arg;
	//Check auth and kick user if not authenticated
	int auth = GetAUTH(sock);
	if(auth != 1){
		fprintf(stderr, "User is not authenticated! exiting...\n");
		send(sock, auth, sizeof(int), 0);
		pthread_exit(NULL);
	}
	else{
		send(sock, auth, sizeof(int), 0);
	}
	while(1);

}

int GetAUTH(int socket_id){
	char auth[100] = "";
	recv(socket_id, &auth, 100, 0);

	FILE *fp;
	char buf[100];
	fp = fopen("Authentication.txt", "r");
	if (fp == NULL) {
		puts("Unable to read Auth file");
		exit(0);
	}
	while (fgets(buf,100, fp)!=NULL){
		char user[100] = {0};
		int d = 0;
		for(int i = 0; i<strlen(buf); i++){
			if(!(isspace(buf[i]))){
				user[d] = buf[i];
				d++;
			}
		}
		if(strcmp(user, auth) == 0){
			printf("yes\n");
			return 1;
		}
	}
	return 0;
}

int main(int argc, char const *argv[]){
	if(argc!=2){
		fprintf(stderr,"usage: port_number\n");
		exit(1);
	}
	else{
		PORT = atoi(argv[1]);
	}

	signal(SIGINT, handle_sigint); 
	sockfd=CreateSocket();
	bzero(&my_addr, sizeof(my_addr));
	GenrateEP();
	BindListen(sockfd);

	while(1) {  /* main accept() loop */

		int client_sock;
		int *new_sock;
		sin_size = sizeof(struct sockaddr_in);
	    while((con_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size))){
	    	pthread_t client_thread;
	        new_sock = malloc(sizeof *new_sock);
	        *new_sock = con_fd;
			pthread_create( &client_thread , NULL , ClientGame, (void*) new_sock);
    	}

    	if(con_fd < 0){
    		perror("Accept Failed");
    		return 1;
    	}
    }
	return 0;
}

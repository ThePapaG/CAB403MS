//CAB403 Assignment 2018 SEM2 - Server
//Written by Callum Scott n9695443
//in conjunction with Grant Dare n9476512

/* Include */

#include <server.h>

/* Variables */
int sockfd, con_fd;  /* listen on sock_fd, new connection on con_fd */
char *message;
struct sockaddr_in my_addr;    /* host address information */
struct sockaddr_in their_addr; /* connector's address information */
socklen_t sin_size;

void handle_sigint(int sig) 
{ 
    printf("Caught signal %d\n", sig);
    printf("Exiting has begun.");
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

// void GetAUTH(){
// 		FILE *fp;
// 	    char buf[1000];
// 	    fp = fopen(AUTHFILE, "r");
// 	    if (fp == NULL) {
// 	        puts("Unable to read Auth file");
// 	        exit(0);
// 	    }
// 	    while (fgets(buf,1000, AUTHFILE)!=NULL)
// 	        		printf("%s",buf);
// 	    fclose(AUTHFILE);

// }

int main(int argc, char const *argv[])
{
	signal(SIGINT, handle_sigint); 
	sockfd=CreateSocket();
	GenrateEP();
	BindListen(sockfd);

	while(1) {  /* main accept() loop */

		sin_size = sizeof(struct sockaddr_in);
		if ((con_fd = accept(sockfd, (struct sockaddr *)&their_addr, \
		&sin_size)) == -1) {
			perror("error accept");
			continue;
		}
		printf("server: got connection from %s\n", \
		inet_ntoa(their_addr.sin_addr));;
		message = "12345";
    	write(con_fd, message, strlen(message));
    	recv(con_fd, message, 10, 0);
    }
	return 0;
}

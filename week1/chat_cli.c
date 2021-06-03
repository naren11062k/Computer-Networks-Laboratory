#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <arpa/inet.h>
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

// Function designed for chat between client and server. 
void func(int sockfd) 
{ 
	char buff[MAX]; 
	int n; 

	// infinite loop for chat 
	for (;;) { 

		bzero(buff, sizeof(buff)); 
		
		printf("Enter the string:"); 
		n = 0;  
		while ((buff[n++] = getchar()) != '\n')	
			;
		write(sockfd, buff, sizeof(buff)); 
		
		bzero(buff, sizeof(buff));
		
		read(sockfd, buff, sizeof(buff));
		
		printf("From Server: %s",buff);

		if ((strncmp(buff, "exit", 4)) == 0) { 
			printf("Client Exit...\n\n\n\n\n\n"); 
			break; 
		}
	}
}

// Driver function 
int main() 
{ 
	int sockfd, connfd; 
	struct sockaddr_in servaddr, cli; 
	
	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
 	if (sockfd == -1) { 
 		printf("socket creation failed...\n"); 
 		exit(0); 
 	} 
 	
	else
 		printf("\n\n\n\n\nSocket successfully created..\n"); 
 	
	bzero(&servaddr, sizeof(servaddr)); 
	
	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
 	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
 	servaddr.sin_port = htons(PORT); 
	
	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
 		printf("connection with the server failed...\n"); 
 		exit(0); 
	}
	
	else
 		printf("connected to the server..\n"); 
	
	// Function for chatting between client and server 
	func(sockfd); 
	
	// After chatting close the socket 
	close(sockfd);
}

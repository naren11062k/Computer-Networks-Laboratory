#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#define SIZE 1024

int main()
{
	int sockfd,connfd,n=0;
	struct sockaddr_in serv_addr;
	char buff[SIZE];

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(7500);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.2");
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		printf("Socket Creation Unsuccessfull!!");
		return 1;
	}

	connfd = connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	while(1)
	{
		printf("ENTER YOUR LINK:");
		scanf("%s",buff);
		write(sockfd,buff,strlen(buff));
		n = read(sockfd,buff,SIZE);
		buff[n]='\0';
		printf("%s\n\n",buff);
	}
}	
	

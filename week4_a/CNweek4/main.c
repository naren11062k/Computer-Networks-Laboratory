#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#define SIZE 1024
char buff[SIZE];

void get(char*buff,char*name,char*date)
{
	int i=0,j=0,k=0;
	while(buff[i]!=' ')
		i++;
	while(buff[++i] !='\n')
	{
		name[j]=buff[i];
		j++;
	}
	name[j]='\0';
	if(buff[++i]!='\0')
	{
		while(buff[i]!=' ')
			i++;
		while(buff[++i]!='\n')
		{
			date[k]=buff[i];
			k++;
		}
	}
	date[k]='\0';
}

int main()
{
	int sockfd,connfd,n=0;
	struct sockaddr_in serv_addr;
	char link[100];
	char ab[100];
	char d[100];

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(6500);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		printf("Socket Creation Unsuccessfull!!\n");
		return 1;
	}

	if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
	{
		printf("Binding Unsuccessful\n");
		return 1;
	}
	
	if(listen(sockfd,5)<0)
	{
		printf("Listening Unsuccessful\n");
		return 1;
	}

	connfd = accept(sockfd,(struct sockaddr*)NULL,NULL);
	if(connfd)
		printf("Proxy Server Established!!\n");

	FILE*fp;

	while(1)
	{
		bzero(buff,SIZE);
		n=read(connfd,buff,SIZE);
		if(n>0)
		{
			bzero(d,100);
			buff[n]='\0';
			printf("Proxy Server Requests:\n%s\n",buff);
			get(buff,link,ab);
			fp=fopen("main.txt","r");
			while(fgets(buff,SIZE,fp)!=NULL)
			{
				if((strncmp(link,buff,strlen(link))==0))
				{		
					fseek(fp,-(strlen(buff)-strlen(link)-1),SEEK_CUR);
					fgets(d,100,fp);
					break;
				}
			}
			printf("%s\n",d);	
			if(strlen(d)==0)
				snprintf(buff,SIZE,"%s\n","Not Found!!");
			else if(strncmp(ab,d,strlen(d)-1)==0)
				snprintf(buff,SIZE,"%s\n","NULL");
			else
				snprintf(buff,SIZE,"Last-Modified: %s\n",d);
			sleep(2);
			write(connfd,buff,strlen(buff));
		}	
	}
}


	

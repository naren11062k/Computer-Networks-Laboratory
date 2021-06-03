#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#define SIZE 1024

void req(char* buff, char* link, char* date)
{
	if(strlen(date)==0)
		snprintf(buff,SIZE,"GET %s\n",link);
	else
		snprintf(buff,SIZE,"GET %s\nIF-MODIFIED-SINCE %s\n",link,date);
}

int upgrade(char* fname, char* buff)
{
	FILE* fp=fopen("p1.txt","r+");
	char c;
	char line[50],date[20];
	int a=0,b=0;
	while(buff[a]!=' ')
		a++;
	while(buff[++a]!='\n')
	{
		date[b]=buff[a];
		b++;
	}
	date[b]='\0';
	while((fgets(line,50,fp))!=NULL)
	{
		if((strncmp(fname,line,strlen(fname)))==0)
		{
			fseek(fp,-(strlen(line)-strlen(fname)-1),SEEK_CUR);
			fputs(date,fp);
			goto EXIT;
		}
	}
	snprintf(buff,SIZE,"%s,%s\n",fname,date);
	fputs(buff,fp);

EXIT:
	fclose(fp);
	printf("THE PROXY SERVER HAS BEEN UPDATED!!\n");
	return 0;
}

int main()
{
	int sockfd1,sockfd2,connfd1,connfd2=0,n=0;
	struct sockaddr_in serv_addr1;
	struct sockaddr_in serv_addr2;
	char buff[SIZE];
	char link[100];
	char d[100];
	FILE*fp;
	bzero(buff,SIZE);

	serv_addr1.sin_family = AF_INET;
	serv_addr1.sin_port = htons(6500);
	serv_addr1.sin_addr.s_addr = inet_addr("127.0.0.1");

	serv_addr2.sin_family = AF_INET;
	serv_addr2.sin_port = htons(7500);                                                                                                                                                                   serv_addr2.sin_addr.s_addr = htonl(INADDR_ANY);

	if((sockfd1=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Socket1 Creation Unsuccessful!!!\n");
		return 1;
	}
	if((sockfd2=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Socket2 Creation Unsuccessful!!!\n");
		return 1;
	}

	connfd1 = connect(sockfd1,(struct sockaddr*)&serv_addr1,sizeof(serv_addr1));
	if(bind(sockfd2,(struct sockaddr*)&serv_addr2,sizeof(serv_addr2))<0)
	{
		printf("Binding Unsuccessful!!\n");
		return 1;
	}
	if(listen(sockfd2,3)<0)
	{
		printf("Listening Unsuccessful!!\n");
		return 1;
	}
	if(connfd2 = accept(sockfd2,(struct sockaddr*)NULL,NULL))
	{
		printf("Client Connection Established\n");
	}
	while(1)
	{
		n=read(connfd2,link,100);
		if(n>0)
		{
			bzero(d,100);
			bzero(buff,SIZE);
			link[n]='\0';
			fp = fopen("p1.txt","r");
			while(fgets(buff,SIZE,fp))
			{
				if(strncmp(link,buff,strlen(link))==0)
				{
					fseek(fp,-(strlen(buff)-strlen(link)-1),SEEK_CUR);
					fgets(d,100,fp);
					break;
				}
				strcpy(d,"");
			}
			fclose(fp);
			req(buff,link,d);
			write(sockfd1,buff,strlen(buff));
R:
			n=read(sockfd1,buff,SIZE);
			if(n>0)
			{
				buff[n]='\0';
				printf("Origin Server Responds:\n%s\n",buff);
			}
			else
				goto R;

			if(strncmp(buff,"NULL",4)==0)
				write(connfd2,"Proxy Server",strlen("Proxy Server"));
			else if(strncmp(buff,"Last",4)==0)
			{
				upgrade(link,buff);
				write(connfd2,"Main Server",strlen("Main Server"));
			}
			else
				write(connfd2,"Not Found!!",strlen("Not Found!!"));
			bzero(buff,SIZE);
		}
	}
}




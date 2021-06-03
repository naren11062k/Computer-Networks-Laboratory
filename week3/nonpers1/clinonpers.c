#include<stdio.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<netinet/in.h>
#include <arpa/inet.h>
int main()
{
        struct sockaddr_in cli;
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0)  { printf("Error in creating socket."); return 0; }
        cli.sin_family = AF_INET;
        cli.sin_port = htons(4000);
        cli.sin_addr.s_addr = inet_addr("127.0.0.1");

        int cli_connect = connect(sockfd, (struct sockaddr *)&cli, sizeof(cli));
        if(cli_connect<0) { printf("Error in connecting."); return 0; }

        char name[40],readmsg[1000],get_header[1000];
        printf("Enter filename: ");
        scanf("%s",name);
        strcpy(get_header,"");
        strcat(get_header,"GET: ");
        strcat(get_header,name);
        //printf("%s",get_header);

        write(sockfd,name,sizeof(name));
        write(sockfd,get_header,sizeof(get_header));

        read(sockfd,readmsg,sizeof(readmsg));
        printf("%s",readmsg);

}


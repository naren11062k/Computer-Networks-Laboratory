#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<time.h>
#include<string.h>
int main()
{
        struct sockaddr_in serveraddr;
        int listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(listensockfd < 0)    { printf("Error in creating socket."); return 0; }
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(4000);
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

        int server_bind = bind(listensockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
        if(server_bind) { printf("Error in binding."); return 0; }

        int server_listen = listen(listensockfd,10);
        if(server_listen == -1) { printf("ERROR IN LISTENING !!"); return 0; }

 while(1)
        {
        int connection = accept(listensockfd, (struct sockaddr*)NULL, NULL);
        if(connection < 0) { printf("ERROR IN CONNECTION !!"); return 0; }
        printf("\nCONNECTED TO CLIENT SERVER !! \n");

        char filename[1000],readmsg[1000],headermsg[1000],res_msg[1000];
        read(connection,filename,sizeof(filename));
        read(connection,headermsg,sizeof(headermsg));
        printf("\nHTTP REQUEST:  ");
        printf("%s\n",filename);

        FILE *fp;
        fp = fopen(filename,"r");
        fread(readmsg,1000,1,fp);
        char timedate[1000];
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        strcpy(timedate,asctime(tm));
        printf("TIME : %s \n", timedate);
        printf("\nCONTENTS OF THE FILE  %s \n%s",filename,readmsg);

                strcpy(res_msg,"");
                strcat(res_msg,"HTTP RESPONSE MESSAGE: ");
                strcat(res_msg,"\nTimestamp: ");
                strcat(res_msg,timedate);
                strcat(res_msg,"MESSAGE ");
                strcat(res_msg,readmsg);

                write(connection,res_msg,sizeof(res_msg));
        }
}

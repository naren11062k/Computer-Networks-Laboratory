#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define size 1024

int main()
{
    int sockfd, sockfd2, n, len;
    char send_buff[size], recv_buff[size];
    char *hello = "hello from server";
    struct sockaddr_in servaddr, servaddr2, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("Socket Creation failed\n");
        return 1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(7000);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("Binding error\n");
        return 1;
    }

    len = sizeof(cliaddr);

    while (1)
    {
        char *ip;
        n = recvfrom(sockfd, recv_buff, size, 0, (struct sockaddr *)&cliaddr, &len);
        recv_buff[n] = '\0';
        printf("\nclient: %s\n", recv_buff);

        if (strncmp(recv_buff, "CLOSE", 5) == 0)
        {
            close(sockfd);
            return 0;
        }
        char filename[20];
        int j = 0;
        for (int i = strlen(recv_buff) - 3; i < strlen(recv_buff); i++, j++)
            filename[j] = recv_buff[i];

        filename[j] = '\0';
        strcat(filename, "_server_1.txt");
        bzero(send_buff, size);
        FILE *fp = fopen(filename, "r");
        if (fp == NULL)
        {
            strcpy(ip, "Invalid Domain Name\n");
        }
        else
        {
            char line[50];
            while (fgets(line, size, fp))
            {
                if (strncmp(line, recv_buff, strlen(recv_buff)) == 0)
                {
                    ip = strtok(line, ":");
                    ip = strtok(NULL, "$");
                    printf("IP Found: ");
                    break;
                }
                else
                {
                    strcpy(ip, "NOT FOUND");
                }
            }
        }
        printf("%s\n", ip);
        sendto(sockfd, ip, strlen(ip), 0, (struct sockaddr *)&cliaddr, len);
    }
    return 0;
}
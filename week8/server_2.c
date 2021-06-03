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
    int sockfd, n, len;
    char buffer[size];
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("Socket Creation failed\n");
        return 1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(7500);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("Binding error\n");
        return 1;
    }

    len = sizeof(cliaddr);
    while (1)
    {
        char url[20], *ip;
        n = recvfrom(sockfd, url, 20, 0, (struct sockaddr *)&cliaddr, &len);
        url[n] = '\0';
        if (strncmp(url, "CLOSE", 5) == 0)
        {
            close(sockfd);
            return 0;
        }
        printf("\nURL From Client: %s\n", url);

        char filename[20];
        int j = 0;
        for (int i = strlen(url) - 3; i < strlen(url); i++, j++)
            filename[j] = url[i];
        filename[j] = '\0';
        strcat(filename, "_server_2.txt");

        FILE *fp = fopen(filename, "r");
        while (fgets(buffer, size, fp))
        {
            if (strncmp(buffer, url, strlen(url)) == 0)
            {
                ip = strtok(buffer, ":");
                ip = strtok(NULL, "$");
                printf("IP Found: %s\n", ip);
                break;
            }
            else
            {
                strcpy(ip, "NOT FOUND");
            }
        }
        sendto(sockfd, ip, strlen(ip), 0, (struct sockaddr *)&cliaddr, len);
    }
    return 0;
}
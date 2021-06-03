#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define SIZE 1024

int main()
{
    int sockfd, n, len;
    char buffer[SIZE];
    char *hola = "hola from server_2";
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("Socket Creation failed\n");
        return 1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(9500);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("binding error\n");
        return 1;
    }

    len = sizeof(cliaddr);
    while (1)
    {
        char url[20], *ipaddr;
        n = recvfrom(sockfd, url, 20, 0, (struct sockaddr *)&cliaddr, &len);
        url[n] = '\0';
        printf("\nURL from server_1: %s\n", url);

        n = recvfrom(sockfd, buffer, SIZE, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        printf("filenames from server_1: %s\n", buffer);
        char *filename = strtok(buffer, ",");

        FILE *fp = fopen(filename, "r");
        while (fgets(buffer, SIZE, fp))
        {
            if (strncmp(buffer, url, strlen(url)) == 0)
            {
                ipaddr = strtok(buffer, ":");
                ipaddr = strtok(NULL, "\n");
                printf("IP ADDRESS: %s\n", ipaddr);
                break;
            }
            else
            {
                strcpy(ipaddr, "NOT FOUND");
            }
        }
        sendto(sockfd, ipaddr, strlen(ipaddr), 0, (struct sockaddr *)&cliaddr, len);
    }

    return 0;
}

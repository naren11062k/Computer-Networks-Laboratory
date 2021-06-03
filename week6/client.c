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
    int sockfd, a, length;
    char buff[SIZE];
    char *hola = "hola from client";
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("Socket creation failed\n");
        return 1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9000);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    while (1)
    {
        printf("\nEnter Host Name: ");
        scanf("%s", buff);
        sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

        a = recvfrom(sockfd, buff, SIZE, 0, (struct sockaddr *)&servaddr, &length);
        buff[a] = '\0';
        printf("IP ADDRESS: %s\n", buff);
    }
    close(sockfd);
    return 0;
}

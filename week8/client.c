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
    char buffer[size], url[20];
    struct sockaddr_in servaddr, servaddr2;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("Socket creation failed\n");
        return 1;
    }
    sockfd2 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd2 < 0)
    {
        printf("Socket 2 Creation failed\n");
        return 1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(7000);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    servaddr2.sin_family = AF_INET;
    servaddr2.sin_addr.s_addr = INADDR_ANY;
    servaddr2.sin_port = htons(7500);

    while (1)
    {
        printf("\nEnter the URL: ");
        scanf("%s", url);
        sendto(sockfd, url, strlen(url), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

        if (strncmp(url, "CLOSE", 5) == 0)
        {
            sendto(sockfd2, url, strlen(url), 0, (struct sockaddr *)&servaddr2, sizeof(servaddr2));
            close(sockfd);
            return 0;
        }

        n = recvfrom(sockfd, buffer, size, 0, (struct sockaddr *)&servaddr, &len);
        buffer[n] = '\0';
        printf("server_1: %s\n", buffer);
        if (strncmp(buffer, "NOT FOUND", 9) == 0)
        {
            sendto(sockfd2, url, strlen(url), 0, (struct sockaddr *)&servaddr2, sizeof(servaddr2));
            n = recvfrom(sockfd2, buffer, size, 0, (struct sockaddr *)&servaddr2, &len);
            buffer[n] = '\0';
            printf("server_2: %s\n", buffer);
        }
    }
    close(sockfd);
    return 0;
}
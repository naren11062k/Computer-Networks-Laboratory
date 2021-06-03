#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define size 1024

int main()
{
    int sockfd, connfd, n = 0;
    struct sockaddr_in serv_addr;
    char recvbuffer[size], sendbuffer[size];

    bzero(recvbuffer, size);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(7500);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Socket creation failed!\n");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Binding failed!!\n");
        return 1;
    }

    if (listen(sockfd, 5) < 0)
    {
        printf("Listening failed\n");
        return 1;
    }

    connfd = accept(sockfd, (struct sockaddr *)NULL, NULL);
    if (connfd)
    {
        printf("Client Connected Successfully!\n");
    }

    while (1)
    {
        n = read(connfd, recvbuffer, size);
        if (n == 0)
            return 0;
        recvbuffer[n] = '\0';
        printf("Client: %s\n", recvbuffer);
        snprintf(sendbuffer, size, "ACK %s", recvbuffer);
        write(connfd, sendbuffer, strlen(sendbuffer));
    }
    return 0;
}

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
    int sockfd, sockfd1, n, len;
    char send_buffer[SIZE], receive_buffer[SIZE];
    char *hola = "hola from server_1";
    struct sockaddr_in servaddr, servaddr1, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("Socket Creation failed\n");
        return 1;
    }

    sockfd1 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd1 < 0)
    {
        printf("Socket 1 Creation failed\n");
        return 1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(9000);

    servaddr1.sin_family = AF_INET;
    servaddr1.sin_addr.s_addr = INADDR_ANY;
    servaddr1.sin_port = htons(9500);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("binding error\n");
        return 1;
    }

    len = sizeof(cliaddr);

    while (1)
    {

        n = recvfrom(sockfd, receive_buffer, SIZE, 0, (struct sockaddr *)&cliaddr, &len);
        receive_buffer[n] = '\0';
        printf("\nclient: %s\n", receive_buffer);

        char file_name[20];
        int j = 0;
        for (int i = strlen(receive_buffer) - 3; i < strlen(receive_buffer); i++, j++)
            file_name[j] = receive_buffer[i];

        file_name[j] = '\0';
        strcat(file_name, "_domain.txt");
        bzero(send_buffer, SIZE);
        FILE *fp = fopen(file_name, "r");
        while (fgets(send_buffer + strlen(send_buffer), SIZE, fp))
            ;
        fclose(fp);
        sendto(sockfd1, receive_buffer, strlen(receive_buffer), 0, (struct sockaddr *)&servaddr1, sizeof(servaddr1));
        sendto(sockfd1, send_buffer, strlen(send_buffer), 0, (struct sockaddr *)&servaddr1, sizeof(servaddr1));

        n = recvfrom(sockfd1, receive_buffer, SIZE, 0, (struct sockaddr *)&servaddr1, &len);
        receive_buffer[n] = '\0';
        printf("server_2: %s\n", receive_buffer);

        sendto(sockfd, receive_buffer, strlen(receive_buffer), 0, (struct sockaddr *)&cliaddr, len);
    }
    return 0;
}

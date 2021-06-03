#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int main()
{
    int sockfd, portno = 5000, n;
    struct sockaddr_in serv_addr;
    
    // Socket Creation
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("\n ERROR opening socket");
        return 0;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Socket Connection
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n ERROR in connecting");
        return 0;
    }

    // Buffers
    char buffer[3000],url[512],send_proxy[512];

    while (1)
    {
        // Resetting to zero
        bzero(buffer, 3000);
        bzero(send_proxy, 512);

        // Reading initial message from server
        n = read(sockfd, buffer, 3000);
        if (n < 0)
        {
            printf("\n Read error");
            return 0;
        }
        printf("%s \n", buffer);

        // Getting user input
        scanf("%s", url);

        printf("\nRequest sent to Proxy Server: \nGET %s HTTP/1.1\n", url);
        strcat(send_proxy, url);
        strcat(send_proxy, "$GET ");
        strcat(send_proxy, url);
        strcat(send_proxy, " HTTP/1.1");

        // Sending request to Proxy Server
        write(sockfd, send_proxy, sizeof(send_proxy));
        if (strcmp(url, "exit") == 0)
        {
            return 0;
        }

        // Reading Response from Proxy Server
        bzero(buffer, 3000);
        n = read(sockfd, buffer, 3000);
        if (n < 0)
        {
            printf("\n Read error");
            return 0;
        }
        printf("\nResponse Message from Server:\n%s\n", buffer);
        printf("\n--------------------\n");
    }
    close(sockfd);
    return 0;
}
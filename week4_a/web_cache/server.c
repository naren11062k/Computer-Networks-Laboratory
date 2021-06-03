#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <fcntl.h>

int main()
{
    int sockfd, server_sockfd, portno = 5001, n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    if (sockfd < 0)
    {
        printf("Socket not created\n");
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Binding failed\n");
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    server_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (server_sockfd < 0)
    {
        printf("New Socket not created\n");
    }

    // Buffers and all
    char from_proxy[512];
    char *token, ftoken[512];
    char *status, *url, *date, *data, *request_body;
    char url_token[512], date_token[512];
    char send_proxy[1024];
    bool flag;
    FILE *fp;

    while (1)
    {
        // Resetting
        bzero(from_proxy, 512);
        bzero(send_proxy, 1024);
        token = NULL;
        data = NULL;
        status = NULL;
        url = NULL;
        date = NULL;
        request_body = NULL;
        fp = NULL;
        flag = false;

        // Read request from Proxy server
        n = read(server_sockfd, from_proxy, 512);
        if (n < 0)
        {
            printf("Error in write");
            return 0;
        }

        // Split status
        token = strtok(from_proxy, "$");
        status = token;

        // Split date
        token = strtok(NULL, "$");
        data = token;

        // Split request body
        token = strtok(NULL, "$");
        request_body = token;

        printf("\nRequest from Proxy Server: \n%s\n", request_body);

        // Split url and date
        if (strcmp(status, "miss") == 0)
        {
            url = data;
        }
        else
        {
            token = strtok(data, "?");
            url = token;
            token = strtok(NULL, "?");
            date = token;
        }

        // Opening file and comparing
        fp = fopen("server_db.txt", "r");

        while (fgets(ftoken, 512, fp) != NULL)
        {
            bzero(url_token, 512);
            bzero(date_token, 512);
            int url_token_index = 0, i = 0, date_token_index = 0;
            for (i = 0; ftoken[i] != '?'; i++)
            {
                url_token[url_token_index++] = ftoken[i];
            }
            i++;
            for (; ftoken[i] != '\0'; i++)
            {
                date_token[date_token_index++] = ftoken[i];
            }

            if (strcmp(url_token, url) == 0)
            {
                flag = true;
                // If Miss send url and date
                if (strcmp(status, "miss") == 0)
                {
                    printf("\nResponse Message sent to Proxy Server:\nHTTP/1.1 200 OK\nDate: %s", date_token);
                    strcat(send_proxy, "200$");
                    strcat(send_proxy, url_token);
                    strcat(send_proxy, "?");
                    strcat(send_proxy, date_token);
                    strcat(send_proxy, "$HTTP/1.1 200 OK\nDate: ");
                    strcat(send_proxy, date_token);
                }
                // Else
                else
                {
                    // If Modified send updated date
                    if (strcmp(date, date_token) == 0)
                    {
                        printf("Response Message sent to Proxy Server:\nHTTP/1.1 304 Not Modified\nDate: %s", date_token);
                        strcat(send_proxy, "notmodified$HTTP/1.1 304 Not Modified\nDate: ");
                        strcat(send_proxy, date_token);
                    }
                    // Else
                    else
                    {
                        printf("Response Message sent to Proxy Server:\nHTTP/1.1 200 OK\nModified\nDate: %s", date_token);
                        strcat(send_proxy, "modified$");
                        strcat(send_proxy, date_token);
                        strcat(send_proxy, "$HTTP/1.1 200 OK\nModified\nDate: ");
                        strcat(send_proxy, date_token);
                    }
                }

                // Send Response to Proxy Server
                n = write(server_sockfd, send_proxy, strlen(send_proxy));
                fclose(fp);
                break;
            }
        }

        // Flag false means invalid url
        if (flag == false)
        {
            printf("CAME HERE\n\n");
            printf("Response Message sent to Proxy Server:\nHTTP/1.1 404 Not Found\n");
            n = write(server_sockfd, "404$HTTP/1.1 404 Not Found", strlen("404$HTTP/1.1 404 Not Found"));
        }
        printf("\n--------------------\n");
    }
    close(server_sockfd);
    close(sockfd);
    return 0;
}

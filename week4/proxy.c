#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

int main()
{
    int sockfd, server_sockfd, portno = 5000, n;

    // Socket Creation
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

    // Bind socket to Port
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Binding failed\n");
    }

    // Listen
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // Accept Connection
    server_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (server_sockfd < 0)
    {
        printf("New Socket not created\n");
    }

    // Another Socket AND Stuff

    int client_sockfd, client_portno = 5001, client_n;
    client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client_serv_addr;

    if (client_sockfd < 0)
    {
        printf("Socket not created\n");
    }

    client_serv_addr.sin_family = AF_INET;
    client_serv_addr.sin_port = htons(client_portno);
    client_serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(client_sockfd, (struct sockaddr *)&client_serv_addr, sizeof(client_serv_addr)) < 0)
    {
        printf("Connection failed\n");
    }

    // Buffers and all

    char url[512];
    char token[512], url_token[512], date_token[512];
    int cache_size;
    bool flag = false;

    char from_server[512];
    char from_client[512];
    char send_server[1024];
    char server_response[20];
    char modified_date[100];
    char send_client[1024];
    char file_edit_buff[512];
    FILE *fp, *fedit, *ftemp;
    char paar[512];
    bool page_found;
    char server_response_body[512];

    while (1)
    {
        // Resetting to zero
        bzero(from_client, 512);
        fp = NULL;
        fedit = NULL;
        ftemp = NULL;
        bzero(url, 512);

        // SEND TO CLIENT
        n = write(server_sockfd, "Enter url:(exit to quit)", strlen("Enter url:(exit to quit)") + 10);
        if (n < 0)
        {
            printf("\n ERROR in socket creation");
            return 0;
        }

        // RECEIVE REQUEST FROM CLIENT
        read(server_sockfd, from_client, sizeof(from_client));
        if(strcmp(from_client,"exit")==0)
        {
            printf("Closing link...\n");
            write(client_sockfd, from_client,strlen(send_server));
            return 0;
        }
        // Extract URL and Request seperately
        int temp = 0, i = 0;
        for (; from_client[i] != '$'; i++)
        {
            url[temp++] = from_client[i];
        }

        i++;
        temp = 0;
        for (; from_client[i] != '\0'; i++)
        {
            from_client[temp++] = from_client[i];
        }
        from_client[temp] = '\0';

        printf("\nRequest From Client:\n%s\n", from_client);

        // Cache Open
        fp = fopen("proxy_db.txt", "r");

        // RESET AND INITIALIZATION
        flag = false;
        bzero(send_server, 1024);
        bzero(from_server, 512);
        bzero(server_response, 20);
        bzero(modified_date, 100);
        bzero(send_client, 1024);
        bzero(token, 512);
        bzero(file_edit_buff, 512);
        bzero(server_response_body, 512);
        bzero(paar, 512);
        page_found = true;

        // Reading cache line by line
        while (fgets(token, 512, fp) != NULL)
        {
            bzero(url_token, 512);
            bzero(date_token, 512);
            // SPLIT URL AND DATE FROM TOKEN
            int url_token_index = 0, i = 0, date_token_index = 0;
            for (i = 0; token[i] != '?'; i++)
            {
                url_token[url_token_index++] = token[i];
            }
            i++;
            for (; token[i] != '\0'; i++)
            {
                date_token[date_token_index++] = token[i];
            }

            // COMPARE SPLITTED URL AND CLIENT URL
            // IF SAME
            if (strcmp(url_token, url) == 0)
            {
                flag = true;
                strcat(send_server, "hit$");
                strcat(send_server, token);
                strcat(send_server, "$GET ");
                strcat(send_server, url_token);
                strcat(send_server, "\nIf-modified-since: ");
                strcat(send_server, date_token);

                // SEND REQUEST TO ORIGIN SERVER
                printf("\nRequest sent to Origin Server:\nGET %s HTTP/1.1\nHost: Localhost\nIf-modified-since: %s\n", url_token, date_token);
                client_n = write(client_sockfd, send_server, strlen(send_server));
                if (client_n < 0)
                {
                    printf("Error in write");
                    return 0;
                }

                // READ RESPONSE FROM ORIGIN SERVER
                client_n = read(client_sockfd, from_server, 512);
                if (client_n < 0)
                {
                    printf("\n Error in read");
                    return 0;
                }

                // SPLIT THE response
                int index = 0;
                i = 0;
                for (; from_server[i] != '$'; i++)
                {
                    server_response[index++] = from_server[i];
                }

                // IF RESPONSE is modified, get modified date
                if (strcmp(server_response, "modified") == 0)
                {
                    // Split the modified date
                    i++;
                    index = 0;
                    for (; from_server[i] != '$'; i++)
                    {
                        modified_date[index++] = from_server[i];
                    }
                    strcat(url_token, "?");
                    strcat(url_token, modified_date);

                    // Update Cache
                    fedit = fopen("proxy_db.txt", "r");
                    ftemp = fopen("replace.tmp", "w");
                    int count = 0;

                    while (fgets(file_edit_buff, 512, fedit) != NULL)
                    {
                        if (strcmp(file_edit_buff, token) == 0)
                        {
                            fputs(url_token, ftemp);
                        }
                        else
                        {
                            fputs(file_edit_buff, ftemp);
                        }
                    }

                    fclose(fedit);
                    fclose(ftemp);

                    rename("replace.tmp", "proxy_db.txt");

                    // Split response and send to Client
                    i++;
                    index = 0;
                    for (; from_server[i] != '\0'; i++)
                    {
                        server_response_body[index++] = from_server[i];
                    }
                    printf("\nResponse from Origin Server:\n%s", server_response_body);
                    printf("\nResponse to Client:\n%s\n", server_response_body);
                    strcat(send_client, server_response_body);
                }
                // ELSE
                else
                {
                    //Split response and send to client
                    i++;
                    index = 0;
                    for (; from_server[i] != '\0'; i++)
                    {
                        server_response_body[index++] = from_server[i];
                    }
                    printf("\nResponse from Origin Server:\n%s\n", server_response_body);
                    printf("\nResponse to Client:\n%s\n", server_response_body);
                    strcat(send_client, server_response_body);
                }
                break;
            }
        }

        // FLAG FALSE MEANS CACHE MISS
        if (flag == false)
        {
            strcat(send_server, "miss$");
            strcat(send_server, url);

            strcat(send_server, "$GET ");
            strcat(send_server, url);
            strcat(send_server, " HTTP/1.1");

            // SEND REQUEST TO ORIGIN SERVER
            printf("\nRequest sent to Origin Server:\nGET %s HTTP/1.1\n", url);
            client_n = write(client_sockfd, send_server, strlen(send_server));
            if (client_n < 0)
            {
                printf("Error in write");
                return 0;
            }

            // READ RESPONSE FROM ORIGIN SERVER
            client_n = read(client_sockfd, from_server, 512);
            if (client_n < 0)
            {
                printf("\n Error in read");
                return 0;
            }

            // Split response and check
            int index = 0, i = 0;
            for (; from_server[i] != '$'; i++)
            {
                server_response[index++] = from_server[i];
            }

            // IF 404, Page not found
            if (strcmp(server_response, "404") == 0)
            {
                // Split Response and Send to Client
                i++;
                index = 0;
                for (; from_server[i] != '\0'; i++)
                {
                    server_response_body[index++] = from_server[i];
                }

                printf("\nResponse From Origin Server:\n%s", server_response_body);
                printf("\nResponse to Client:\n%s", server_response_body);
                strcat(send_client, server_response_body);
            }
            // Else store in cache
            else
            {
                // Split url and date
                i++;
                index = 0;
                for (; from_server[i] != '$'; i++)
                {
                    server_response_body[index++] = from_server[i];
                }

                // Store in cache
                fedit = fopen("proxy_db.txt", "a");

                fputs(server_response_body, fedit);

                fclose(fedit);

                // Split response and send to client
                bzero(server_response_body, 512);
                i++;
                index = 0;
                for (; from_server[i] != '\0'; i++)
                {
                    server_response_body[index++] = from_server[i];
                }
                printf("\nResponse From Origin Server:\n%s", server_response_body);
                printf("\nResponse to Client:\n%s", server_response_body);
                strcat(send_client, server_response_body);
            }
        }

        // SEND RESPONSE TO CLIENT
        n = write(server_sockfd, send_client, strlen(send_client));
        if (n < 0)
        {
            printf("Error in write");
            return 0;
        }
        fclose(fp);
        printf("\n--------------------\n");
    }
    close(server_sockfd);
    close(sockfd);
    close(client_sockfd);

    return 0;
}

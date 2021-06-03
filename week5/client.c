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
    int control_sockfd, control_portno = 5000, n;
    struct sockaddr_in control_addr;

    // Socket Creation
    control_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (control_sockfd < 0)
    {
        printf("\n ERROR opening socket");
        return 0;
    }
    control_addr.sin_family = AF_INET;
    control_addr.sin_port = htons(control_portno);
    control_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Socket Connection
    if (connect(control_sockfd, (struct sockaddr *)&control_addr, sizeof(control_addr)) < 0)
    {
        printf("\n ERROR in connecting");
        return 0;
    }

    // Buffers
    char send_proxy[512];
    char from_server[1024];
    char username[512], password[512], send_server[1024];

    while (1)
    {
        // Resetting to zero
        bzero(send_server, 512);
        bzero(from_server, 1024);

        printf("Enter username and password: (exit to quit)\n");

        scanf("%s", username);
        if (strcmp(username, "exit") == 0)
        {
            printf("Closing Program\n");
            write(control_sockfd,"exit",strlen("exit"));
            close(control_sockfd);
            exit(0);
        }
        scanf("%s", password);

        strcat(send_server, username);
        strcat(send_server, "$");
        strcat(send_server, password);

        write(control_sockfd, send_server, sizeof(send_server));

        bzero(from_server, 1024);
        n = read(control_sockfd, from_server, 1024);

        if (strcmp(from_server, "valid") == 0)
        {
            printf("Authentication Successful\n");
            int data_sockfd, data_portno = 5001, m;
            struct sockaddr_in data_addr;

            // Socket Creation
            data_sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (data_sockfd < 0)
            {
                printf("\n ERROR opening socket");
                return 0;
            }
            data_addr.sin_family = AF_INET;
            data_addr.sin_port = htons(data_portno);
            data_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

            // Socket Connection
            if (connect(data_sockfd, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0)
            {
                printf("\n ERROR in connecting");
                return 0;
            }

            // Buffers
            char data_from_server[512], data_send_server[512], choice[100];

            while (1)
            {
                bzero(data_from_server, 512);
                bzero(data_send_server, 512);
                bzero(choice, 100);

                printf("\nEnter the filename:\n");

                scanf("%s", data_send_server);
                if(strcmp(data_send_server,"exit") == 0) {
                    printf("Closing File Layer\n");
                    write(data_sockfd,"exit",strlen("exit"));
                    close(data_sockfd);
                    break;
                }

                write(data_sockfd, data_send_server, strlen(data_send_server));

                bzero(data_from_server, 512);
                read(data_sockfd, data_from_server, sizeof(data_from_server));

                if (strcmp(data_from_server, "File exist") == 0)
                {
                    printf("File exists\nEnter 'YES' to display contents:\n");
                    scanf("%s", choice);
                    if (strcmp(choice, "YES") == 0)
                    {
                        write(data_sockfd, "show", strlen("show"));
                        bzero(data_from_server, 512);
                        read(data_sockfd, data_from_server, sizeof(data_from_server));
                        printf("%s contains %s\n", data_send_server,data_from_server);
                    }
                    else
                    {
                        write(data_sockfd, "dontshow", strlen("dontshow"));
                    }
                }
                else
                {
                    printf("%s\n", data_from_server);
                }
            }
            close(data_sockfd);
        }
        else
        {
            printf("\nInvalid Username/Password. Try again\n\n");
        }
    }
    close(control_sockfd);
    return 0;
}

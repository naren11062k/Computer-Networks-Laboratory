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

    int sockfd, control_sockfd, control_portno = 5000, n;

    // Socket Creation
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in control_addr, cli_addr;
    socklen_t clilen;

    if (sockfd < 0)
    {
        printf("Socket not created\n");
    }
    control_addr.sin_family = AF_INET;
    control_addr.sin_port = htons(control_portno);
    control_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind socket to Port
    if (bind(sockfd, (struct sockaddr *)&control_addr, sizeof(control_addr)) < 0)
    {
        printf("Binding failed\n");
    }

    // Listen
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // Accept Connection
    control_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (control_sockfd < 0)
    {
        printf("New Socket not created\n");
    }

    char from_client[1024];
    char username[512], password[512], ftoken[512], username_token[512], password_token[512];
    bool user_found;
    char send_client[1024];
    FILE *fptr;

    while (1)
    {
        bzero(from_client, 1024);
        bzero(send_client, 1024);
        bzero(username, 512);
        bzero(password, 512);
        bzero(ftoken, 512);
        fptr = NULL;
        user_found = false;

        read(control_sockfd, from_client, sizeof(from_client));
        if (strcmp(from_client, "exit") == 0)
        {
            close(control_sockfd);
            close(sockfd);
            printf("Closing Server...\n");
            exit(0);
        }

        int index = 0, i = 0;
        for (; from_client[i] != '$'; i++)
        {
            username[index++] = from_client[i];
        }
        i++;
        index = 0;
        for (; from_client[i] != '\0'; i++)
        {
            password[index++] = from_client[i];
        }

        printf("Credentials sent from Client\nUsername: %s\nPassword: %s\n", username, password);

        fptr = fopen("credentials.txt", "r");

        while (fgets(ftoken, 512, fptr) != NULL)
        {
            index = 0;
            i = 0;
            for (; ftoken[i] != '$'; i++)
            {
                username_token[index++] = ftoken[i];
            }
            username_token[index] = '\0';
            i++;
            index = 0;
            for (; ftoken[i] != '\n'; i++)
            {
                password_token[index++] = ftoken[i];
            }
            password_token[index] = '\0';

            if (strcmp(username, username_token) == 0 && strcmp(password, password_token) == 0)
            {
                user_found = true;
                break;
            }
            else
            {
                user_found = false;
            }
        }

        if (user_found == true)
        {
            printf("\nUsername and Password successfully Authenticated\n");
            n = write(control_sockfd, "valid", strlen("valid"));

            int d_sockfd, data_sockfd, control_portno = 5001, m;

            // Socket Creation
            d_sockfd = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in control_addr, cli_addr;
            socklen_t clilen;

            if (d_sockfd < 0)
            {
                printf("Socket not created\n");
            }
            control_addr.sin_family = AF_INET;
            control_addr.sin_port = htons(control_portno);
            control_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

            // Bind socket to Port
            if (bind(d_sockfd, (struct sockaddr *)&control_addr, sizeof(control_addr)) < 0)
            {
                printf("Binding failed\n");
            }

            // Listen
            listen(d_sockfd, 5);
            clilen = sizeof(cli_addr);

            // Accept Connection
            data_sockfd = accept(d_sockfd, (struct sockaddr *)&cli_addr, &clilen);
            if (data_sockfd < 0)
            {
                printf("New Socket not created\n");
            }

            // Buffers
            char data_from_client[512], data_ftoken[512], file_content[512], filename_without_newline[512];
            FILE *data_fptr, *data_fptr_read;
            bool file_exists;

            while (1)
            {
                bzero(data_from_client, 512);
                bzero(data_ftoken, 512);
                bzero(file_content, 512);
                bzero(filename_without_newline, 512);
                data_fptr = NULL;
                data_fptr_read = NULL;
                file_exists = false;

                read(data_sockfd, data_from_client, sizeof(data_from_client));
                if (strcmp(data_from_client, "exit") == 0)
                {
                    printf("Closing File Layer...");
                    close(data_sockfd);
                    close(d_sockfd);
                    break;
                }

                printf("\nRequested File: %s\n",data_from_client);
                strncpy(filename_without_newline, data_from_client, 512);
                strcat(data_from_client, "\n");

                data_fptr = fopen("files.txt", "r");

                while (fgets(data_ftoken, 512, data_fptr) != NULL)
                {
                    if (strcmp(data_from_client, data_ftoken) == 0)
                    {
                        file_exists = true;
                        break;
                    }
                    else
                    {
                        file_exists = false;
                    }
                }

                fclose(data_fptr);

                if (file_exists == true)
                {
                    printf("\nFile exists\n");
                    m = write(data_sockfd, "File exist", strlen("File exist"));
                    bzero(data_from_client, 512);
                    read(data_sockfd, data_from_client, sizeof(data_from_client));

                    if (strcmp(data_from_client, "show") == 0)
                    {
                        printf("\nUser Requests File Content\n");
                        int fd = open(filename_without_newline, O_RDONLY);
                        if (fd < 0)
                        {
                            printf("ERROR\n");
                        }

                        read(fd, file_content, 512);

                        printf("%s\n", file_content);
                        close(fd);
                        m = write(data_sockfd, file_content, strlen(file_content));
                    }
                    else
                    {
                        printf("\nUser not requesting File Content\n");
                    }
                }
                else
                {
                    printf("\nFile doesn't exist\n");
                    m = write(data_sockfd, "File doesn't exist", strlen("File doesn't exist"));
                }
            }
            close(data_sockfd);
            close(d_sockfd);
        }
        else
        {
            printf("\nInvalid Username and Password\n");
            n = write(control_sockfd, "invalid", strlen("invalid"));
        }
    }

    close(control_sockfd);
    close(sockfd);
}
